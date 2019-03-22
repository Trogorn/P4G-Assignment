#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include "GeometryBuilder.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;



void Game::OnResize(int screenWidth, int screenHeight)
{
	extern bool mMaximized;
	if (mMaximized)
		screenWidth = screenHeight = screenWidth;
	OnResize_Default(screenWidth, screenHeight);
}

void Game::Initialise()
{
	mFX.Init(gd3dDevice);

	//main floor
	mQuad1.Initialise(BuildQuad(mMeshMgr));
	MaterialExt mat = mQuad1.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = mFX.mCache.LoadTexture("building1.dds", true, gd3dDevice);
	mQuad1.GetPosition() = Vector3(0, 0, 0);
	mQuad1.GetRotation() = Vector3(0, 0, 0);
	mQuad1.GetScale() = Vector3(3, 1, 3);
	mQuad1.SetOverrideMat(&mat);

	//main floor
	mQuad2.Initialise(BuildQuad(mMeshMgr));
	mat = mQuad2.GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX.mCache.LoadTexture("building2.dds", true, gd3dDevice);
	mQuad2.GetPosition() = Vector3(0, 0, 0);
	mQuad2.GetRotation() = Vector3(0, 0, 0);
	mQuad2.GetScale() = Vector3(3, 1, 3);
	mQuad2.SetOverrideMat(&mat);

	//Skyscraper
	mCube.Initialise(BuildCube(mMeshMgr));
	mCube.GetScale() = Vector3(0.25f, 0.5f, 0.25f);
	mCube.GetPosition() = Vector3(0.0f, 0.5f, 0.5f);
	mat = mCube.GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX.mCache.LoadTexture("building1.dds", true, gd3dDevice);
	mat.texture = "building1.dds"; //text label for debugging
	mCube.SetOverrideMat(&mat);
	//Block of flats

	mFlats.clear();
	mFlats.insert(mFlats.begin(), 100, mCube);
	for (int i = 0; i < 100; i++) {
	mFlats[i].GetScale() = Vector3(0.1f, 0.25f, 0.1f);
	mFlats[i].GetPosition() = Vector3(GetRandom(-2.f,1.f), 0, GetRandom(-2.f, 1.f));
	mFlats[i].GetRotation() = Vector3(0, GetRandom(0.f, 2 * PI), 0);
	switch (GetRandom(0, 3)) {
	case 0:
		mat.pTextureRV = mFX.mCache.LoadTexture("building1.dds", true, gd3dDevice);
		break;
	case 1:
		mat.pTextureRV = mFX.mCache.LoadTexture("building2.dds", true, gd3dDevice);
		break;
	case 2:
		mat.pTextureRV = mFX.mCache.LoadTexture("building3.dds", true, gd3dDevice);
		break;
	case 3:
		mat.pTextureRV = mFX.mCache.LoadTexture("building4.dds", true, gd3dDevice);
		break;
	}
	mCube.SetOverrideMat(&mat);
	}
	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(1, 1, 1), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

}

void Game::Release()
{
	mFX.Release();
	mMeshMgr.Release();
}

void Game::Update(float dTime)
{

}

void Game::Render(float dTime)
{
	BeginRender(Colours::Black);

	FX::SetPerFrameConsts(gd3dImmediateContext, mCamPos);

	CreateViewMatrix(FX::GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, GetAspectRatio(), 1, 1000.f);

	float alpha = 0.5f + sinf(gAngle * 2)*0.5f;

	//point light
	//mLight.GetPosition() = Vector3(-2 + alpha * 4, 0.5, 1);
	//mLight.GetRotation() = Vector3(1, 0, 0);
	//Vector3 plightColour = Vector3(alpha, 0.7f, 0.7f);
	//FX::SetupPointLight(3, true, Vector3(5,0,1), Vector3(0.7f, 0.7f, 0.7f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0, 0, 0), 1, 9.f);



	//floor
	mFX.Render(mQuad1, gd3dImmediateContext);
	mFX.Render(mQuad2, gd3dImmediateContext);
	mFX.Render(mCube, gd3dImmediateContext); // Skyscraper
	for (int i = 0; i < (int)mFlats.size(); ++i) //Flats
		mFX.Render(mFlats[i], gd3dImmediateContext);


	EndRender();
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 100.f * GetElapsedSec();
	//do something game specific here
	switch (msg)
	{
		// Respond to a keyboard event.
	case WM_CHAR:
		switch (wParam)
		{
		case 27:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			return 0;
		case 'w':
			mCamPos.y += camInc;
			break;
		case 's':
			mCamPos.y -= camInc;
			break;
		case 'a':
			mCamPos.x -= camInc;
			break;
		case 'd':
			mCamPos.x += camInc;
			break;
		case 'z':
			mCamPos.z += camInc;
			break;
		case 'x':
			mCamPos.z -= camInc;
			break;
		case ' ':
			mCamPos = mDefCamPos;
			break;
		}
	}
	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}


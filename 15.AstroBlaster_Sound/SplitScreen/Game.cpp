#include <algorithm>

#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include "GeometryBuilder.h"
#include "FX.h"
#include "Input.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Vector3 gWorldScale(10, 10, 10);
Matrix gViewMat1, gViewMat2;


void Game::OnResize(int screenWidth, int screenHeight)
{
	OnResize_Default(screenWidth, screenHeight);
}

void Game::Initialise()
{
	mQuad.Initialise(BuildQuad(*GetMeshManager()));

	//textured lit box
	mBox.Initialise(BuildCube(*GetMeshManager()));
	mBox.GetPosition() = Vector3(0, -0.5f, 1);
	mBox.GetScale() = Vector3(0.5f, 0.5f, 0.5f);
	MaterialExt mat = mQuad.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.5, 0.5, 0.5, 1), Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("test.dds", true, gd3dDevice);
	mat.texture = "test.dds";
	mBox.SetOverrideMat(&mat);


	// floor
	mQuad.GetScale() = Vector3(3, 1, 3);
	mQuad.GetPosition() = Vector3(0, -1, 0);
	mat = mQuad.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("test.dds", true, gd3dDevice);
	mat.texture = "test.dds";
	mat.texTrsfm.scale = Vector2(10, 10);
	mQuad.SetOverrideMat(&mat);

	Mesh& sb = GetMeshManager()->CreateMesh("skybox");
	sb.CreateFrom("data/skybox.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mSkybox.Initialise(sb);
	mSkybox.GetScale() = Vector3(1,1,1);
	mSkybox.GetPosition() = Vector3(0,0,0);
	mSkybox.GetRotation() = Vector3(PI/2,0,0);
	MaterialExt& defMat = mSkybox.GetMesh().GetSubMesh(0).material;
	defMat.flags &= ~MaterialExt::LIT;
	defMat.flags &= ~MaterialExt::ZTEST;

	Mesh& gm = GetMeshManager()->CreateMesh("gun");
	gm.CreateFrom("data/gun/colt_python.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mGun.Initialise(gm);
	//mGun = mBox;
	mGun.GetScale() = Vector3(0.1,0.1,0.1);
	mGun.GetPosition() = Vector3(2, -1, 5);
	mGun.GetRotation() = Vector3(PI/2, 0, 0);

	//scale the world
	mOpaques.push_back(&mQuad);
	mOpaques.push_back(&mBox);
	for (Model* obj : mOpaques)
	{
		obj->GetScale() *= gWorldScale;
		obj->GetPosition() *= gWorldScale;
	}


	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.67f, 0.67f, 0.67f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.15f, 0.15f, 0.15f));


	// Sets up my camera biiiatch
	mCamera.Initialise(Vector3(-5, -4.5, -15), Vector3(0, 0, 0), gViewMat1);
	mCamera.LockMovementAxis(FPSCamera::UNLOCK, -4.5f, FPSCamera::UNLOCK);

	mCamera2.Initialise(Vector3(-10, -4.5, 20), Vector3(PI, 0, 0), gViewMat2);
	mCamera2.LockMovementAxis(FPSCamera::UNLOCK, -4.5f, FPSCamera::UNLOCK);
}

void Game::Release()
{
}

void Game::Update(float dTime)
{

	gAngle += dTime * 0.5f;
	mBox.GetRotation().y = gAngle;
}

void Game::Render(float dTime)
{
	BeginRender(Colours::Black);


	//screen resolution
	int sw, sh;
	GetClientExtents(sw, sh);
	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, sw / (sh*0.5f), 1, 1000.f);


	//Grabs the first view matrix that camera is linked to
	FX::GetViewMatrix() = gViewMat1;
	//setup the first viewport
	SetViewportDimensions(gd3dImmediateContext, 0, 0, sw, sh/2, gScreenViewport);
	FX::SetPerFrameConsts(gd3dImmediateContext, mCamera.GetPos());
	//Render Shit
	mSkybox.GetPosition() = mCamera.GetPos();
	FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);
	for (Model*p : mOpaques)
		FX::GetMyFX()->Render(*p, gd3dImmediateContext);
	
	


	//Grabs the second view matrix that camera is linked to
	FX::GetViewMatrix() = gViewMat2;
	//setup the second viewport
	SetViewportDimensions(gd3dImmediateContext, 0, sh/2, sw, sh / 2, gScreenViewport);
	FX::SetPerFrameConsts(gd3dImmediateContext, mCamera2.GetPos());


	//Render Shit again
	mSkybox.GetPosition() = mCamera2.GetPos();
	FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);
	for (Model*p : mOpaques)
		FX::GetMyFX()->Render(*p, gd3dImmediateContext);
	


	EndRender();

	GetMouseAndKeys()->PostProcess();
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 200.f * GetElapsedSec();

	switch (msg)
	{
	case WM_INPUT:
		GetMouseAndKeys()->MessageEvent((HRAWINPUT)lParam);
		break;
		// Respond to a keyboard event.
	case WM_CHAR:
		switch (wParam)
		{
		case 27:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			return 0;
		}
	}

	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}


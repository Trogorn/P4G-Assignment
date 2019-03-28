#include <sstream>
#include <thread>

#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include "GeometryBuilder.h"
#include "CommonStates.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


std::vector<BoundingBox> boxColiders;

void Game::OnResize(int screenWidth, int screenHeight)
{
	
	OnResize_Default(screenWidth, screenHeight);
}


void Game::Load()
{
	//wood floor
	mQuad.Initialise(BuildQuad(mMeshMgr));
	MaterialExt *pMat = &mQuad.GetMesh().GetSubMesh(0).material;
	pMat->gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 10));
	pMat->pTextureRV = mFX.mCache.LoadTexture("floor.dds", true, gd3dDevice);
	pMat->texture = "floor";
	mLoadData.loadedSoFar++;

	//torch
	mTorch.Initialise(BuildCube(mMeshMgr));
	pMat = &mTorch.GetMesh().GetSubMesh(0).material;
	pMat->gfxData.Set(Vector4(1, 0, 0, 1), Vector4(1, 0, 0, 1), Vector4(1, 0, 0, 1));
	mTorch.GetScale() = Vector3(0.2f, 0.1f, 0.1f);
	mTorch.GetPosition() = Vector3(0, 0.2f, 0);
	mLoadData.loadedSoFar++;

	Mesh& cb = mMeshMgr.CreateMesh("Cube");
	cb.CreateFrom("../bin/data/two_mat_cube.fbx", gd3dDevice, mFX.mCache);
	mCube.Initialise(cb);
	mCube.GetScale() = Vector3(0.05f, 0.05f, 0.05f);
	mCube.GetPosition() = Vector3(-1.5f,0,0);
	mCube.GetRotation() = Vector3(PI/2,0,0);
	mLoadData.loadedSoFar++;

	Mesh& ms = mMeshMgr.CreateMesh("rock");
	ms.CreateFrom("../bin/data/the_rock/TheRock2.obj", gd3dDevice, mFX.mCache);
	mRock.Initialise(ms);
	mRock.GetScale() = Vector3(0.005f, 0.005f, 0.005f);
	mRock.GetPosition() = Vector3(1.5f, 0, 0);
	mRock.GetRotation() = Vector3(0, 0, 0);
	mLoadData.loadedSoFar++;

}

void Game::LoadDisplay(float dTime)
{
	BeginRender(Colours::Black);

	mpSpriteBatch->Begin();

	static int pips = 0;
	static float elapsed = 0;
	elapsed += dTime;
	if (elapsed > 0.25f){
		pips++;
		elapsed = 0;
	}
	if (pips > 10)
		pips = 0;
	wstringstream ss;
	ss << L"Loading meshes(" << (int)(((float)mLoadData.loadedSoFar / (float)mLoadData.totalToLoad)*100.f) << L"%) ";
	for (int i = 0; i < pips; ++i)
		ss << L'.';
	mpFont2->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(100, 200), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));


	ss.str(L"");
	ss << L"FPS:" << (int)(1.f / dTime);
	mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));


	mpSpriteBatch->End();


	EndRender();

}

void Game::Initialise()
{
	mFX.Init(gd3dDevice);


	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.8f,0.8f,0.8f), Vector3(0.15f,0.15f,0.15f), Vector3(0.25f,0.25f,0.25f));

	mpSpriteBatch = new SpriteBatch(gd3dImmediateContext);
	assert(mpSpriteBatch);
	mpFont = new SpriteFont(gd3dDevice, L"../bin/data/comicSansMS.spritefont");
	assert(mpFont);

	mpFont2 = new SpriteFont(gd3dDevice, L"../bin/data/algerian.spritefont");
	assert(mpFont2);

	mLoadData.totalToLoad = 4;
	mLoadData.loadedSoFar = 0;
	mLoadData.running = true;
	mLoadData.loader = std::async(launch::async, &Game::Load, this);

	mMKInput.Initialise(GetMainWnd(), false);
	mGamepad.Initialise();
}

void Game::Release()
{
	mFX.Release();
	mMeshMgr.Release();
	delete mpFont;
	mpFont = nullptr;
	delete mpSpriteBatch;
	mpSpriteBatch = nullptr;
	delete mpFont2;
	mpFont2 = nullptr;
}




void Game::Update(float dTime)
{
	mGamepad.Update();
	mTorch.GetRotation().y = GetClock();
}




void Game::Render(float dTime)
{
	if (mLoadData.running)
	{
		if (!mLoadData.loader._Is_ready())
		{
			LoadDisplay(dTime);
			return;
		}
		mLoadData.loader.get();
		mLoadData.running = false;
		return;
	}

	BeginRender(Colours::Black);

	FX::SetPerFrameConsts(gd3dImmediateContext, mCamPos);

	CreateViewMatrix(FX::GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, GetAspectRatio(), 1, 1000.f);


	//the torch
	mFX.Render(mTorch, gd3dImmediateContext);
	Vector3 dir(1, 0, 0);
	dir = Vector3::TransformNormal(dir, Matrix::CreateRotationY(mTorch.GetRotation().y));
	FX::SetupSpotLight(2, true, mTorch.GetPosition(), dir, Vector3(1,0,0), Vector3(0.1f, 0.1f, 0.1f), Vector3(0.1f, 0, 0.1f), 3.f, 0.05f);

	//floor
	mQuad.GetPosition() = Vector3(0, 0, 0);
	mQuad.GetRotation() = Vector3(0, 0, 0);
	mQuad.GetScale() = Vector3(3, 1, 3);
	mFX.Render(mQuad, gd3dImmediateContext);

	mCube.GetPosition() = hitPosition;
	mFX.Render(mCube, gd3dImmediateContext);
	mFX.Render(mRock, gd3dImmediateContext);


	//we'll be using alpha transparency and it isn't pre-multiplied alpha
	//so we need to tweak how sprites are rendered
	CommonStates state(gd3dDevice);
	mpSpriteBatch->Begin(SpriteSortMode_Deferred, state.NonPremultiplied());

	
	//general messages
	wstring wmssg = L"Mesh loading complete";
	mpFont->DrawString(mpSpriteBatch, wmssg.c_str(), Vector2(10, 10), Colours::White, 0, Vector2(0, 0), Vector2(1,1));
	
	wstringstream ss;
	if (dTime > 0)
		ss << L"FPS: " << (int)(1.f / dTime);
	else
		ss << L"FPS: 0";
	mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));


	mpSpriteBatch->End();


	EndRender();

	mMKInput.PostProcess();
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 20.f * GetElapsedSec();
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
		case ' ':
			mCamPos = mDefCamPos;
			break;
		}
	case WM_INPUT:
			mMKInput.MessageEvent((HRAWINPUT)lParam);
			break;
	}
	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

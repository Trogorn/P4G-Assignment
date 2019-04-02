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




void Game::OnResize(int screenWidth, int screenHeight)
{
	
	OnResize_Default(screenWidth, screenHeight);
}


void Game::Load()
{	
	mCar.Initialise(BuildCube(mMeshMgr));
	*mCar.GetScale() = Vector3(1.f, 1.f, 1.f);
	*mCar.GetPosition() = Vector3(0,0,0);
	*mCar.GetRotation() = Vector3(0,0,0);

	mLoadData.loadedSoFar++;

}

//Loading Screen
void Game::LoadDisplay(float dTime)
{
	BeginRender(Colours::Blue);

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

//First Function called in the Game Class, Sets up the class
void Game::Initialise()
{

	mFX.Init(gd3dDevice);


	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.8f,0.8f,0.8f), Vector3(0.15f,0.15f,0.15f), Vector3(0.25f,0.25f,0.25f));

	mpSpriteBatch = new SpriteBatch(gd3dImmediateContext);
	assert(mpSpriteBatch);
	mpFont = new SpriteFont(gd3dDevice, L"../bin/data/comicSansMS.spritefont");
	assert(mpFont);

	//pFontBatch2 = new SpriteBatch(gd3dImmediateContext);
	//assert(pFontBatch2);
	mpFont2 = new SpriteFont(gd3dDevice, L"../bin/data/algerian.spritefont");
	assert(mpFont2);

	mLoadData.totalToLoad = 1;
	mLoadData.loadedSoFar = 0;
	mLoadData.running = true;

	mLoadData.loader = std::async(launch::async, &Game::Load, this);

	mMKInput.Initialise(GetMainWnd(), false);
	mGamepad.Initialise();

	player.Initialise(&mMKInput, &mGamepad, &mCar, 90, 0.01f, 0.1f);	


}

//Called at the end releases memory
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

//Called Every Frame
void Game::Update(float dTime)
{
	if (mLoadData.running)
	{
		return;
	}
	mGamepad.Update();

	const float speed = 5;


	//if (mMKInput.IsPressed(VK_A))
	//{
	//	mCamPos.x -= speed * dTime;
	//}

	//if (mMKInput.IsPressed(VK_D))
	//{
	//	mCamPos.x += speed * dTime;
	//}

	/*if (mMKInput.IsPressed(VK_S))
	{
		mCamPos.z -= speed * dTime;
	}

	if (mMKInput.IsPressed(VK_W))
	{
		mCamPos.z += speed * dTime;
	}*/

	if (mMKInput.IsPressed(VK_R))
	{
		mCamPos.y += speed * dTime;
	}

	if (mMKInput.IsPressed(VK_F))
	{
		mCamPos.y -= speed * dTime;
	}
	mCamPos.y += mGamepad.GetState(0).leftStickY * dTime * speed;
	mCamPos.x += mGamepad.GetState(0).leftStickX * dTime * speed;
	mCamPos.z += mGamepad.GetState(0).rightStickY * dTime * speed;

	player.Update(dTime);
	player.UpdateCamera(dTime);
	//mCamPos = player.GetCameraPos();

}

//Called at the end of Every Frame
void Game::Render(float dTime)
{
	//Loading Screen Check
	if (mLoadData.running)//Am I currently Running
	{
		//Yes, the check if loading is finished
		if (!mLoadData.loader._Is_ready())
		{
			//If No then display Loading Screen
			LoadDisplay(dTime);
			//Return (Don't do anything else in this function
			return;
		}
		//IDK what this is
		mLoadData.loader.get();
		//Loader has finished loading (is ready) so set running boolean to false (aka not running)
		mLoadData.running = false;
		//Return (Don't try to load anything on this run through wait for the next frame before attempting to render anything)
		return;
	}

	//Render Stuff After this
	BeginRender(Colours::Blue);

	//So Sort of constants in the GPU not really sure what this is?
	FX::SetPerFrameConsts(gd3dImmediateContext, mCamPos);

	////Create Camera Matrix
	//CreateViewMatrix(FX::GetViewMatrix(), mCamPos, *player.GetPosition(), Vector3(0, 1, 0));
	////Create Projection Matrix to be applied to the Camera
	//CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, GetAspectRatio(), 1, 1000.f);	 

	CommonStates state(gd3dDevice);
	//Setup Sprite Batch
	mpSpriteBatch->Begin(SpriteSortMode_Deferred, state.NonPremultiplied());
	wstringstream ss;
	if (dTime > 0)
		ss << L"FPS: " << (int)(1.f / dTime);
	else
		ss << L"FPS: 0";
	player.Debug();
	ss << MyDebug::GetDebugMessage().c_str();
	mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 0), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));


	//End Sprite Batch
	mpSpriteBatch->End();
	
	player.Render(mFX);


	EndRender();

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
		//If the event is an input we want then run the Input classes Message Event function (gets the inputs and sets the buffers for later retrieval)
	case WM_INPUT:
			mMKInput.MessageEvent((HRAWINPUT)lParam);
			break;
	}
	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}


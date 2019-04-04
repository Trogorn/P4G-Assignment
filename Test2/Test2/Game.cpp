#include "Game.h"


using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;




Matrix gViewMat1, gViewMat2;
Vector3 AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid) {
	/*AvoidQuad parameters explained
float y: the actual Y value at which you want to spawn
float minX, maxX: the minimum and maximum value for X in the TOTAL map quad
float minZ, maxZ:  the minimum and maximum value for Z in the TOTAL map quad
float minXAvoid, maxXAvoid: the minimum and maximum value for X in the AVOIDED quad
float minZAvoid, maxZAvoid:  the minimum and maximum value for Z in the AVOIDED quad

while loop keeps generating X and Z values until the point is not within the avoided quad

The vector 3 returned has float y parameter and an X and Z value outside the avoided quad
*/
	
	float returnX = minXAvoid;
	float returnZ = minZAvoid;
	while ((returnX <= maxXAvoid && returnX >= minXAvoid) && (returnZ <= maxZAvoid && returnZ >= minZAvoid)) {
		returnX = GetRandom(minX, maxX);
		returnZ = GetRandom(minZ, maxZ);
	}
	return Vector3(returnX, y, returnZ);
}


void Game::OnResize(int screenWidth, int screenHeight)
{
	extern bool mMaximized;
	if (mMaximized)
		screenWidth = screenHeight = screenWidth;
	OnResize_Default(screenWidth, screenHeight);
}

void Game::Load()
{
	//ANY MODEL LOADING GOES HERE NOWHERE ELSE----------------------------------------------------------------

	mCar.Initialise(BuildCube(mMeshMgr));
	*mCar.GetScale() = Vector3(0.05f, 0.05f, 0.1f);
	*mCar.GetPosition() = Vector3(0, 0, 0);
	*mCar.GetRotation() = Vector3(0, 0, 0);

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
	if (elapsed > 0.25f) {
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


	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.8f, 0.8f, 0.8f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

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

	//Don't touch above this in the function unless you know what your doing -----------------------------------------------------------------------------------------

	mMKInput.Initialise(GetMainWnd(), false);
	mGamepad.Initialise();

	player.Initialise(&mMKInput, &mGamepad, &mCar, 90, 0.01f, 0.1f, 0.1f, 10.f, 5.f);


	
	mCamera.Initialise(Vector3(0, 1, 0), Vector3(0, 0, 1), gViewMat1);
	mCamera.Funcy(mFlats);
	mMKInput.Initialise(GetMainWnd(), false, true);
	mCamera.LockMovementAxis(Turret::UNLOCK, -9.5f, Turret::UNLOCK);

	//Rob floor
	///*
	mQuad1 = new Model_Kami();
	mQuad2 = new Model_Kami();
	//main floor
	mQuad1->Initialise(BuildQuad(mMeshMgr));
	MaterialExt mat = mQuad1->GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = mFX.mCache.LoadTexture("building1.dds", true, gd3dDevice);
	mat.texture = "building1.dds";
	*mQuad1->GetPosition() = Vector3(0, 0, 0);
	*mQuad1->GetRotation() = Vector3(0, 0, 0);
	*mQuad1->GetScale() = Vector3(3, 1, 3);
	mQuad1->SetOverrideMat(&mat);

	//avoided floor
	mQuad2->Initialise(BuildQuad(mMeshMgr));
	mat = mQuad2->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX.mCache.LoadTexture("building2.dds", true, gd3dDevice);
	mat.texture = "building2.dds";
	*mQuad2->GetPosition() = Vector3(-1, 0.1, -1);
	*mQuad2->GetRotation() = Vector3(0, 0, 0);
	*mQuad2->GetScale() = Vector3(1, 1, 1);
	mQuad2->SetOverrideMat(&mat);
	//*/





	//Skyscraper
	mCube = new Model_Kami();
	mCube->Initialise(mMeshMgr.GetMesh("box"));
	*mCube->GetScale() = Vector3(0.25f, 0.5f, 0.25f);
	*mCube->GetPosition() = Vector3(0.0f, 0.5f, 0.5f);
	mat = mCube->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX.mCache.LoadTexture("building1.dds", true, gd3dDevice);
	mat.texture = "building1.dds"; //text label for debugging
	mCube->SetOverrideMat(&mat);
	//*/

	//Block of flats

	//ROB Block of flats
	// Move this to a function no need to have it all happen here!!!! (Lochlann)
	///*
	SeedRandom(time(NULL));
	mFlats.clear();
	int w = (int)sqrt(mFlats.size());
	//mFlats.insert(mFlats.begin(), 100, mCube);
	for (size_t i = 0; i < 100; i++)
	{
		// Todo - Clear this later on
		auto temp = new Model_Kami();
		temp->Initialise(mMeshMgr.GetMesh("box"));
		temp->SetOverrideMat(&mat);
		mFlats.push_back(temp);
	}
	float xMin = -3;
	float xMax = 3;
	float zMin = -3;
	float zMax = 3;
	float minXAvoid = -2;
	float maxXAvoid = 0;
	float minZAvoid = -2;
	float maxZAvoid = 0;
	float xo = -2 + (float)xMin*0.35f;
	float zo = -1.1 + (float)zMin*0.35f;
	for (int i = 0; i < 100; i++) 
	{
		float rY = GetRandom(0.2f, 0.3f);
		*mFlats[i]->GetScale() = Vector3(0.1f, rY , 0.1f);
		float mFlatX = -0.5;
		float mFlatZ = -0.5;
		while ((mFlatX < 0 && mFlatX > -2) && (mFlatZ < 0 && mFlatZ > -2)) 
		{
			mFlatX = GetRandom(xMin, xMax);
			mFlatZ = GetRandom(zMin, zMax);
		}
		*mFlats[i]->GetPosition() = AvoidQuad(rY, xMin + 0.1f, xMax - 0.1f, zMin + 0.1f, zMax - 0.1f, minXAvoid -.5f, maxXAvoid + .5f, minZAvoid -.5f , maxZAvoid + .5f);
		*mFlats[i]->GetRotation() = Vector3(0, GetRandom(0.f, 2 * PI), 0);
		//mFlats[i]->GetScale() = Vector3(0.1f, rY, 0.1f);
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
		mCube->SetOverrideMat(&mat);

		//*/
	//ELLIOT block of flats
	/*
		mFlats.clear();
		//mFlats.insert(mFlats.begin(), 100, mCube);

		for (size_t i = 0; i < 100; i++)
		{
			// Todo - Clear this later on
			auto temp = new Model_Kami();
			temp->Initialise(mMeshMgr.GetMesh("box"));
			temp->SetOverrideMat(&mat);
			mFlats.push_back(temp);
		}


		int w = (int)sqrt(mFlats.size());




		for (int x = 0; x < w; ++x)
		{
			for (int y = 0; y < w; ++y)

			{
				//mFlats
				float xo = -2 + (float)x*0.35f;
				float zo = -1.1 + (float)y*0.35f;
				float rY = GetRandom(0.2f, 0.3f);

				mFlats[y * w + x]->GetScale() = Vector3(0.1f, rY, 0.1f);
				mFlats[y * w + x]->GetPosition() = Vector3(xo, rY, zo);
				//mFlats[y * w + x].GetRotation() = Vector3(0, GetRandom(0.f, 2 * PI), 0);

				int choice = GetRandom(0, 4);
				switch (choice)
				{
				case 0:
					mat.pTextureRV = mFX.mCache.LoadTexture("building1.dds", true, gd3dDevice);
					break;
				case 1:
					mat.pTextureRV = mFX.mCache.LoadTexture("building2.dds", true, gd3dDevice);
					break;
				case2:
					mat.pTextureRV = mFX.mCache.LoadTexture("building3.dds", true, gd3dDevice);
					break;
				case3:
					mat.pTextureRV = mFX.mCache.LoadTexture("building4.dds", true, gd3dDevice);
					break;
				}

				mCube->SetOverrideMat(&mat);
			}
		}
		*/
		FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(1, 1, 1), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

	}


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

	//Fix the crap ton of memory leaks in the initialise code
}

float x;

void Game::Update(float dTime)
{

	if (mLoadData.running)
	{
		return;
	}

	mGamepad.Update();
	
	x += dTime;
	Vector2 m = mMKInput.GetMouseMoveAndCentre();
	if (m.x != 0 || m.y != 0)
		mCamera.Rotate(dTime, m.x, m.y, 0);
	//if(mCamera.Shoot()!=nullptr)
		//mCamera.Shoot()->Die();
	mCamera.SetPosition(Vector3(0, x, 0));

	player.Update(dTime);
	//may move this to be inside Update instead of outside like it is here
	player.UpdateCamera(dTime);
	
	
}

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



	BeginRender(Colours::Black);

	//Ask Tom about what this is
	int sw, sh;
	GetClientExtents(sw, sh);
	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, sw / (sh*0.5f), 1, 1000.f);
	//////

	FX::GetViewMatrix() = gViewMat1;
	SetViewportDimensions(gd3dImmediateContext, 0, 0, sw, sh / 2, gScreenViewport);
	FX::SetPerFrameConsts(gd3dImmediateContext, mCamera.GetPos());

	float alpha = 0.5f + sinf(gAngle * 2)*0.5f;

	//point light 
	//mLight.GetPosition() = Vector3(-2 + alpha * 4, 0.5, 1);
	//mLight.GetRotation() = Vector3(1, 0, 0);
	//Vector3 plightColour = Vector3(alpha, 0.7f, 0.7f);
	//FX::SetupPointLight(3, true, Vector3(5,0,1), Vector3(0.7f, 0.7f, 0.7f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0, 0, 0), 1, 9.f);



	//floor
	mFX.Render(*mQuad1, gd3dImmediateContext);
	mFX.Render(*mQuad2, gd3dImmediateContext);
	//mFX.Render(*mCube, gd3dImmediateContext); // Skyscraper
	for (int i = 0; i < (int)mFlats.size(); ++i) //Flats
	{
		if(mFlats[i]->RetAlive())
			mFX.Render(*mFlats[i], gd3dImmediateContext);
		mFlats[i]->ColiderUpdate();
	}

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

	player.Render(&mFX);
	
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
		case 'z':
			if(mCamera.Shoot()!=nullptr)
				mCamera.Shoot()->Die();
			break; 
		}
	case WM_INPUT:
		mMKInput.MessageEvent((HRAWINPUT)lParam);
		break;
	}
	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}


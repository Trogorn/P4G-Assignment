
#include <algorithm>
#include "WindowUtils.h"
#include "D3D.h"
#include "GeometryBuilder.h"
#include "Game.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Vector3 gWorldScale(10, 10, 10);
Matrix gViewMat1, gViewMat2;

const int MAP_WIDTH = 64;
const float wscale = 0.09f, SizeOffset = -2.9f;
float offset = 3;
const float mainOffset = offset / MAP_WIDTH + (MAP_WIDTH / 2);
struct MapData
{
	bool road = false;  //red pixels
	bool wall = false;	//green pixel
	bool obstacle = false; // blue pixel
};
MapData map[MAP_WIDTH][MAP_WIDTH];


void Game::OnResize(int screenWidth, int screenHeight)
{
	OnResize_Default(screenWidth, screenHeight);
}

void Game::Initialise()
{
	timer = 0;
	//Set to playing for debug
	gameState = State::MENU;

	mMeshMg = GetMeshManager();
	mFX = FX::GetMyFX();

	BuildNeeded(*mMeshMg);

	FX::SetupDirectionalLight(0, true, Vector3(0.1f, -0.7f, -0.7f), Vector3(1.f, 1.f, 1.f), Vector3(0.15f, 0.15f, 0.15f), Vector3(1.f, 1.f, 1.f));

	mpSpriteBatch = new SpriteBatch(gd3dImmediateContext);
	assert(mpSpriteBatch);
	mpFont = new SpriteFont(gd3dDevice, L"data/comicSansMS.spritefont");
	assert(mpFont);

	mpFont2 = new SpriteFont(gd3dDevice, L"../bin/data/algerian.spritefont");
	assert(mpFont2);

	mLoadData.totalToLoad = 8;
	mLoadData.loadedSoFar = 0;
	mLoadData.running = true;

	mLoadData.loader = std::async(launch::async, &Game::Load, this);

	//Don't touch above this in the function unless you know what your doing -----------------------------------------------------------------------------------------




	mQuad.Initialise(*mMeshMg->GetMesh("quad"));
}

void Game::Release()
{

}

void Game::Load()
{
	//ANY MODEL LOADING GOES HERE NOWHERE ELSE----------------------------------------------------------------

		// car setup
	/*mCar.Initialise(BuildCube(*mMeshMg));
	mCar.GetScale() = Vector3(0.05f, 0.05f, 0.1f);
	mCar.GetPosition() = Vector3(0, 0.05, 0);
	mCar.GetRotation() = Vector3(0, 0, 0);*/
	mCar = new Model();
	mCar->Initialise(mMeshMg->CreateMesh("car"));
	mCar->GetMesh().CreateFrom("data/car/Lambo.FBX", gd3dDevice, FX::GetMyFX()->mCache);
	mCar->GetScale() = Vector3(0.05f, 0.05f, 0.05f);
	mCar->GetPosition() = Vector3(0, 0.02, 0);

	PlayerDrive.Initialise(GetMouseAndKeys(), mCar, 5, 0.5f, 1, 5, 1, 1, 1, &PlayerTurret, &mFlats);

	mLoadData.loadedSoFar++;





	//Load skyscraper
	Floor = new Model();
	AvoidPlane = new Model();
	//main floor
	Floor->Initialise(*mMeshMg->GetMesh("quad"));
	MaterialExt mat = Floor->GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = mFX->mCache.LoadTexture("data/building1.dds", false, gd3dDevice);
	mat.texture = "building1.dds";
	Floor->GetPosition() = Vector3(0, 0, 0);
	Floor->GetRotation() = Vector3(0, 0, 0);
	Floor->GetScale() = Vector3(3, 1, 3);
	Floor->SetOverrideMat(&mat);

	//BobTBuilder.BuildFloor();
	mLoadData.loadedSoFar++;




	//avoided floor
	AvoidPlane->Initialise(*mMeshMg->GetMesh("quad"));
	mat = AvoidPlane->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX->mCache.LoadTexture("data/building2.dds", false, gd3dDevice);
	mat.texture = "building2.dds";
	AvoidPlane->GetPosition() = Vector3(-1, 0.01f, -1);
	AvoidPlane->GetRotation() = Vector3(0, 0, 0);
	AvoidPlane->GetScale() = Vector3(1, 1, 1);
	AvoidPlane->SetOverrideMat(&mat);

	mLoadData.loadedSoFar++;





	//Skyscraper
	mCube = new Model();
	mCube->Initialise(*mMeshMg->GetMesh("cube"));
	mCube->GetScale() = Vector3(0.25f, 0.5f, 0.25f);
	mCube->GetPosition() = Vector3(0.0f, 0.5f, 0.5f);

	mat = mCube->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX->mCache.LoadTexture("data/building1.dds", false, gd3dDevice);
	mat.texture = "building1.dds"; //text label for debugging
	mCube->SetOverrideMat(&mat);

	mLoadData.loadedSoFar++;

	//Turret and Skybox below here================================================================================================

	// floor
	MaterialExt *pMat = &mQuad.GetMesh().GetSubMesh(0).material;
	mQuad.Initialise(*mMeshMg->GetMesh("quad"));
	mQuad.GetScale() = Vector3(3, 1, 3);
	mQuad.GetPosition() = Vector3(0, -1, 0);
	mat = mQuad.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("data/test.dds", false, gd3dDevice);
	mat.texture = "test.dds";
	mat.texTrsfm.scale = Vector2(10, 10);
	mQuad.SetOverrideMat(&mat);

	mLoadData.loadedSoFar++;




	//SkyBox
	Mesh& sb = GetMeshManager()->CreateMesh("skybox");
	sb.CreateFrom("data/skybox.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mSkybox.Initialise(sb);
	mSkybox.GetScale() = Vector3(3, 3, 3);
	mSkybox.GetPosition() = Vector3(0, 0, 0);
	mSkybox.GetRotation() = Vector3(PI / 2, 0, 0);
	MaterialExt& defMat = mSkybox.GetMesh().GetSubMesh(0).material;
	defMat.flags &= ~MaterialExt::LIT;
	defMat.flags &= ~MaterialExt::ZTEST;

	mLoadData.loadedSoFar++;





	ThirdTurret = new Model();
	//Mesh& tm = GetMeshManager()->CreateMesh("3rdgun");
	//tm.CreateFrom("data/Turret.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	//ThirdTurret->Initialise(tm);
	ThirdTurret->Initialise(mMeshMg->CreateMesh("3rdgun"));
	ThirdTurret->GetMesh().CreateFrom("data/Turret.FBX", gd3dDevice, FX::GetMyFX()->mCache);
	//mGun = mBox;
	ThirdTurret->GetScale() = Vector3(0.005, 0.005, 0.005);
	ThirdTurret->GetPosition() = Vector3(0, 0, 0);
	ThirdTurret->GetRotation() = Vector3(0, XMConvertToRadians(180), 0);

	mLoadData.loadedSoFar++;






	FirstTurret = new Model();
	//Mesh& fm = GetMeshManager()->CreateMesh("1stgun");
	//fm.CreateFrom("data/Turret.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	//FirstTurret->Initialise(fm);
	FirstTurret->Initialise(mMeshMg->CreateMesh("1stgun"));
	FirstTurret->GetMesh().CreateFrom("data/Turret.FBX", gd3dDevice, FX::GetMyFX()->mCache);
	//mGun = mBox;
	FirstTurret->GetScale() = Vector3(0.01, 0.01, 0.01);
	FirstTurret->GetPosition() = Vector3(0, -0.05, 0);
	FirstTurret->GetRotation() = Vector3(0, XMConvertToRadians(180), 0);

	mLoadData.loadedSoFar++;






	//Turet person Laser
	mLaz = new Model();
	//Mesh& lm = GetMeshManager()->CreateMesh("Lazer");
	//lm.CreateFrom("data/Lazer.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mLaz->Initialise(mMeshMg->CreateMesh("3rdLaser"));
	mLaz->GetMesh().CreateFrom("data/Lazer.fbx", gd3dDevice, FX::GetMyFX()->mCache);

	mLaz->GetScale() = Vector3(0.003, 0.003, 1);
	mLaz->GetPosition() = Vector3(0, 0.016f, 0);
	mLaz->GetRotation() = Vector3(0, XMConvertToRadians(180), 0);

	//First Person Laser
	mFLaz = new Model();
	mFLaz->Initialise(mMeshMg->CreateMesh("1stLaser"));
	mFLaz->GetMesh().CreateFrom("data/Lazer.fbx", gd3dDevice, FX::GetMyFX()->mCache);

	mFLaz->GetPosition() = Vector3(0, -0.001, 0);
	mFLaz->GetScale() = Vector3(0.001, 0.001, 3);
	mFLaz->GetRotation() = Vector3(0, XMConvertToRadians(180), 0);


	PlayerTurret.Initialise(FirstTurret, ThirdTurret, mFLaz, mLaz, Vector3(0, 1, 0), Vector3(0, 0, 1), gViewMat1);
	PlayerTurret.Funcy(mFlats);

	PlayerTurret.LockMovementAxis(Turret::UNLOCK, -9.5f, Turret::UNLOCK);

	mLoadData.loadedSoFar++;

}

void Game::LoadLevel()
{
	LoadMap();
	mFlats.insert(mFlats.begin(), 100, mCube);
	mFlats.clear();
	for (int xi = 0; xi < MAP_WIDTH; ++xi)
	{
		for (int yi = 0; yi < MAP_WIDTH; ++yi)
		{
			MapData& d = map[MAP_WIDTH - 1 - yi][xi]; //invert it so it's orientated with the world coordinate system
			if (d.obstacle)

			{


				Model* temp = new Model();
				temp->Initialise(*mMeshMg->GetMesh("cube"), true);

				temp->GetPosition() = Vector3(xi*wscale + SizeOffset, 0.1f, yi*wscale + SizeOffset);
				temp->GetScale() = Vector3(0.045f, 0.3f, 0.045f);
				mFlats.push_back(temp);


			}
			mLoadData.loadedSoFar++;
		}
	}
}

//Loading Screen
void Game::StartupDisplay(float dTime)
{
	BeginRender(Colours::Black);

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
		ResetStatesAfterSprites();
}

void Game::LoadDisplay(float dTime)
{
	BeginRender(Colours::Black);

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
	ss << L"Loading Level(" << (int)(((float)mLoadData.loadedSoFar / (float)mLoadData.totalToLoad)*100.f) << L"%) ";
	for (int i = 0; i < pips; ++i)
		ss << L'.';
	mpFont2->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(100, 200), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));


	ss.str(L"");
	ss << L"FPS:" << (int)(1.f / dTime);
	mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 550), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));

	mpSpriteBatch->End();

	EndRender();
	ResetStatesAfterSprites();
}

void Game::Update(float dTime)
{
	if (mLoadData.running)
	{
		return;
	}

	switch (gameState)
	{
	case State::MENU:
		//MENU UPDATE HERE////////////////////////////////////////////////////////////////////////////////////////////////////
		PlayerDrive.MenuUpdate(dTime);
		PlayerTurret.MenuUpdate(dTime);

		if (GetMouseAndKeys()->IsPressed(VK_Z))
		{
			gameState = State::PLAYING;
			PlayerDrive.Reset();
			PlayerTurret.Reset();

			//Start Loading the Level
			mLoadData.totalToLoad = MAP_WIDTH * MAP_WIDTH;
			mLoadData.loadedSoFar = 0;
			mLoadData.running = true;

			mLoadData.loader = std::async(launch::async, &Game::LoadLevel, this);

		}

		break;
	case State::PLAYING:
		//PLAYING UPDATE HERE////////////////////////////////////////////////////////////////////////////////////////////////
		timer += dTime;
		PlayerTurret.Update(dTime);
		gAngle += dTime * 0.5f;

		PlayerDrive.Update(dTime);
		PlayerDrive.UpdateCamera(dTime);

		if (GetMouseAndKeys()->GetMouseButton(MouseAndKeys::LBUTTON))
		{
			PlayerTurret.Shoot();
		}

		break;
	case State::ENDSCREEN:
		//END UPDATE HERE///////////////////////////////////////////////////////////////////////////////////////////////////
		break;
	}

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
			StartupDisplay(dTime);
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

	wstringstream ss;

	int sw, sh;

	switch(gameState)
	{
	case State::MENU:;
		//MENU RENDER HERE/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		BeginRender(Colours::Black);

		GetClientExtents(sw, sh);
		CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, sw / sh, 0.01f, 100.f);
		CreateViewMatrix(FX::GetViewMatrix(), Vector3(0, 0.25, 0.5), Vector3(0, 0.1, 0), Vector3::Up);

		SetViewportDimensions(gd3dImmediateContext, 0, 0, sw, sh, gScreenViewport);
		FX::SetPerFrameConsts(gd3dImmediateContext, PlayerTurret.GetPos());


		

		FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);
		PlayerDrive.Render();
		PlayerTurret.Renderthird();

		mpSpriteBatch->Begin();


		ss << L"Kamakazi Kar!!!";
		mpFont2->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(sw/3, sh/3), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));


		ss.str(L"");
		ss << L"FPS:" << (int)(1.f / dTime);
		mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 10), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));

		ss.str(L"");
		ss << L"Press Z to Start";
		mpFont2->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(sw / 3, sh / 3 + 50), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));

		mpSpriteBatch->End();


		EndRender();
		break;
	case State::PLAYING:
		//PLAYING RENDER HERE/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		BeginRender(Colours::Black);
		ShowCursor(false);

		//screen resolution

		GetClientExtents(sw, sh);

		

		//FIRST PERSON======================================================================================================================================
		CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, sw / (sh*0.5f), 0.01f, 100.f);



		FX::GetViewMatrix() = gViewMat1;

		SetViewportDimensions(gd3dImmediateContext, 0, 0, sw, sh / 2, gScreenViewport);
		FX::SetPerFrameConsts(gd3dImmediateContext, PlayerTurret.GetPos());

		// Render Skybox
		mSkybox.GetPosition() = PlayerTurret.GetPos();
		FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);


		for (int i = 0; i < (int)mFlats.size(); ++i)
		{
			if (mFlats[i]->GetAlive())
				FX::GetMyFX()->Render(*mFlats[i], gd3dImmediateContext);
			mFlats[i]->ColiderUpdate();
		}

		FX::GetMyFX()->Render(*Floor, gd3dImmediateContext);
		FX::GetMyFX()->Render(*AvoidPlane, gd3dImmediateContext);
		//mpSpriteBatch->Begin(SpriteSortMode_Deferred, state.NonPremultiplied());
		//mpSpriteBatch->Draw(FX::GetMyFX() ->mCache.LoadTexture("test.dds", true, gd3dDevice), Vector2(sw / 2, sh / 4), nullptr, Colours::White, 0, Vector2(0, 0), Vector2(0.01f, 0.01f));


		//Render turret view
		PlayerTurret.Renderfirst();
		//Render the car into first person
		PlayerDrive.Render();

		mpSpriteBatch->Begin();

		ss << MyDebug::GetDebugMessage().c_str();
		mpFont2->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 20), Colours::White, 0, Vector2(0, 0), Vector2(1.f, 1.f));

		ss.str(L"");
		ss << L"FPS:" << (int)(1.f / dTime);
		ss << L"\nTime: " << (int)timer;
		mpFont->DrawString(mpSpriteBatch, ss.str().c_str(), Vector2(10, 0), Colours::White, 0, Vector2(0, 0), Vector2(0.5f, 0.5f));

		mpSpriteBatch->End();

		ResetStatesAfterSprites();

		//Switch to third person camera
		PlayerDrive.GetCamera()->Render(FX::GetMyFX(), *PlayerDrive.GetPosition());

		//THIRD PERSON======================================================================================================================================

		// render skybox
		FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);

		for (int i = 0; i < (int)mFlats.size(); ++i)
		{
			if (mFlats[i]->GetAlive())
				FX::GetMyFX()->Render(*mFlats[i], gd3dImmediateContext);
			mFlats[i]->ColiderUpdate();
		}

		FX::GetMyFX()->Render(*Floor, gd3dImmediateContext);
		FX::GetMyFX()->Render(*AvoidPlane, gd3dImmediateContext);

		//render the third person turret
		PlayerTurret.Renderthird();

		//Render the car in third person
		PlayerDrive.Render();



		EndRender();

		//GetMouseAndKeys()->PostProcess();

		break;
	case State::ENDSCREEN:;
		//END RENDER HERE/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		break;
	}


	



}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 200.f * GetElapsedSec();

	switch (msg)
	{
	case WM_INPUT:
		//GetMouseAndKeys()->MessageEvent((HRAWINPUT)lParam);
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

Vector3 Game::AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid) {
	/*
	AvoidQuad parameters explained
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
	while ((returnX <= maxXAvoid && returnX >= minXAvoid) && (returnZ <= maxZAvoid && returnZ >= minZAvoid))
		// Needs to include an initial condition to not be the same X and Z as previous one
	{
		returnX = GetRandom(minX, maxX);
		returnZ = GetRandom(minZ, maxZ);
	}

	return Vector3(returnX, y, returnZ);
}



void Game::LoadMap()
{
	//get the image
	int x, y, n;
	unsigned char *data = stbi_load("../bin/data/TheRock.jpg", &x, &y, &n, 0);
	assert(n == 3);
	//promote to 32bit, DX11 textures are RedGreenBlueAlpha (8bit per channel)
	//but the image was RGB 8bit which is annoying
	unsigned char *data32 = new unsigned char[4 * x*y];
	unsigned char* pSource = data, *pDest = data32;
	for (int i = 0; i < (x*y); ++i)
	{
		for (int ii = 0; ii < 3; ii++)
		{
			*pDest = *pSource;
			pDest++;
			pSource++;
		}
		*pDest = 255;
		pDest++;
	}

	//create a texture to match, yuck, this M$ code is nasty looking
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = x;
	desc.Height = y;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ArraySize = 1;
	ID3D11Texture2D *pTexture = NULL;
	HR(gd3dDevice->CreateTexture2D(&desc, NULL, &pTexture));
	//grab it to write into
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(gd3dImmediateContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	//copy the image data across
	memcpy(mappedResource.pData, data32, sizeof(unsigned char)*x*y * 4);
	gd3dImmediateContext->Unmap(pTexture, 0);
	//free our data, DX has it now
	stbi_image_free(data);
	delete[] data32;
	//convert the texture into a shader resource we can use
	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(resViewDesc));
	resViewDesc.Format = desc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MostDetailedMip = 0;
	resViewDesc.Texture2D.MipLevels = 1;
	//mQuad has not been setup yet at this point in time, it doesn't happen until line 325, this function is called at 119 which is way before that in the execution sequence
	MaterialExt *pMat = &mQuad.GetMesh().GetSubMesh(0).material;
	HR(gd3dDevice->CreateShaderResourceView(pTexture, &resViewDesc, &pMat->pTextureRV));
	//we should really tell the texture cache about this or it won't get deleted :)


	//load the map and convert to a 2d array
	data = stbi_load("../bin/data/TheRockMap.jpg", &x, &y, &n, 0);
	assert(n == 3);
	assert(x == MAP_WIDTH && y == MAP_WIDTH);
	for (int xi = 0; xi < MAP_WIDTH; ++xi)
		for (int yi = 0; yi < MAP_WIDTH; ++yi)
		{
			MapData& d = map[MAP_WIDTH - 1 - yi][xi]; //invert it so it's orientated with the world coordinate system
			d.road = (data[yi*y * 3 + xi * 3] > 200) ? true : false; //red?
			d.wall = (data[yi*y * 3 + xi * 3 + 1] > 200) ? true : false; //green?
			d.obstacle = (data[yi*y * 3 + xi * 3 + 2] > 200) ? true : false; //blue?

		}

	stbi_image_free(data);

}
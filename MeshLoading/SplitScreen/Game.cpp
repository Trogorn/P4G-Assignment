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
	mMeshMg = GetMeshManager();
	mFX = FX::GetMyFX();

	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.8f, 0.8f, 0.8f), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

	mpSpriteBatch = new SpriteBatch(gd3dImmediateContext);
	assert(mpSpriteBatch);
	mpFont = new SpriteFont(gd3dDevice, L"data/comicSansMS.spritefont");
	assert(mpFont);

	mpFont2 = new SpriteFont(gd3dDevice, L"../bin/data/algerian.spritefont");
	assert(mpFont2);

	mLoadData.totalToLoad = 105;
	mLoadData.loadedSoFar = 0;
	mLoadData.running = true;

	mLoadData.loader = std::async(launch::async, &Game::Load, this);

	//Don't touch above this in the function unless you know what your doing -----------------------------------------------------------------------------------------


	mCamera.Initialise(Vector3(0, 1, 0), Vector3(0, 0, 1), gViewMat1);
	mCamera.Funcy(mFlats);

	mCamera.LockMovementAxis(Turret::UNLOCK, -9.5f, Turret::UNLOCK);
	player.Initialise(GetMouseAndKeys(), &mCar, 5, 0.5f, 1, 1, 1, 1, 1, &mCamera);

}

void Game::Release()
{

}

void Game::Load()
{
	//ANY MODEL LOADING GOES HERE NOWHERE ELSE----------------------------------------------------------------


		// car setup
	mCar.Initialise(BuildCube(*mMeshMg));
	mCar.GetScale() = Vector3(0.05f, 0.05f, 0.1f);
	mCar.GetPosition() = Vector3(0, 0.2f, 0);
	mCar.GetRotation() = Vector3(0, 0, 0);

	mLoadData.loadedSoFar++;

	//Load skyscraper
	mQuad1 = new Model();
	mQuad2 = new Model();
	//main floor
	mQuad1->Initialise(BuildQuad(*mMeshMg));
	MaterialExt mat = mQuad1->GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = mFX->mCache.LoadTexture("data/building1.dds", false, gd3dDevice);
	mat.texture = "building1.dds";
	mQuad1->GetPosition() = Vector3(0, 0, 0);
	mQuad1->GetRotation() = Vector3(0, 0, 0);
	mQuad1->GetScale() = Vector3(3, 1, 3);
	mQuad1->SetOverrideMat(&mat);

	//BobTBuilder.BuildFloor();
	mLoadData.loadedSoFar++;

	//avoided floor
	mQuad2->Initialise(*mMeshMg->GetMesh("quad"));
	mat = mQuad2->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX->mCache.LoadTexture("data/building2.dds", false, gd3dDevice);
	mat.texture = "building2.dds";
	mQuad2->GetPosition() = Vector3(-1, 0.01f, -1);
	mQuad2->GetRotation() = Vector3(0, 0, 0);
	mQuad2->GetScale() = Vector3(1, 1, 1);
	mQuad2->SetOverrideMat(&mat);

	mLoadData.loadedSoFar++;

	//Skyscraper
	mCube = new Model();
	mCube->Initialise(*mMeshMg->GetMesh("cube"));
	mCube->GetScale() = Vector3(0.25f, 0.5f, 0.25f);
	mCube->GetPosition() = Vector3(0.0f, 0.5f, 0.5f);
	//*/


	mat = mCube->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX->mCache.LoadTexture("data/building1.dds", false, gd3dDevice);
	mat.texture = "building1.dds"; //text label for debugging
	mCube->SetOverrideMat(&mat);

	mLoadData.loadedSoFar++;

	//Block of flats

	SeedRandom(time(NULL));
	mFlats.clear();
	int w = (int)sqrt(mFlats.size());
	//mFlats.insert(mFlats.begin(), 100, mCube);
	for (size_t i = 0; i < 100; i++)
	{
		// Todo - Clear this later on
		Model* temp = new Model();
		temp->Initialise(*mMeshMg->GetMesh("cube"));
		temp->SetOverrideMat(&mat);
		mFlats.push_back(temp);
		//delete temp;
		//temp = nullptr;
	}
	float xMin = -3;
	float xMax = 3;
	float zMin = -3;
	float zMax = 3;
	float minXAvoid = -2;
	float maxXAvoid = 0;
	float minZAvoid = -2;
	float maxZAvoid = 0;

	/*for (int i = 0; i < 100; ++i)
	{
		bool busy(true);
		while (busy)
		{
			Vector3 newPos = rand();
			busy = false;
			for (auto& obj : mFlats)
			{
				if ((newPos - obj->GetPosition()).Length() < 50)
				{
					busy = true;
					break;
				}
			}
		}
	}*/



	for (int i = 0; i < 100; i++)
	{
		float rY = GetRandom(0.2f, 0.3f);
		mFlats[i]->GetScale() = Vector3(0.1f, rY, 0.1f);
		float mFlatX = -0.5;
		float mFlatZ = -0.5;
		while ((mFlatX < 0 && mFlatX > -2) && (mFlatZ < 0 && mFlatZ > -2))
		{
			//if (i = 0)
			//{
			mFlatX = GetRandom(xMin, xMax);
			mFlatZ = GetRandom(zMin, zMax);
			//}
			//else if (i >= 1 && (mFlatX != mFlats[i - 1]->GetPosition().x && mFlatZ != mFlats[i - 1]->GetPosition().z))
			//{
				//mFlatX = GetRandom(xMin, xMax);
				//mFlatZ = GetRandom(zMin, zMax);
			//}
		}
		mFlats[i]->GetPosition() = AvoidQuad(rY, xMin + 0.1f, xMax - 0.1f, zMin + 0.1f, zMax - 0.1f, minXAvoid - .5f, maxXAvoid + .5f, minZAvoid - .5f, maxZAvoid + .5f);
		mFlats[i]->GetRotation() = Vector3(0, GetRandom(0.f, 2 * PI), 0);
		//mFlats[i]->GetScale() = Vector3(0.1f, rY, 0.1f);
		switch (GetRandom(0, 3)) {
		case 0:
			mat.pTextureRV = mFX->mCache.LoadTexture("data/building1.dds", false, gd3dDevice);
			break;
		case 1:
			mat.pTextureRV = mFX->mCache.LoadTexture("data/building2.dds", false, gd3dDevice);
			break;
		case 2:
			mat.pTextureRV = mFX->mCache.LoadTexture("data/building3.dds", false, gd3dDevice);
			break;
		case 3:
			mat.pTextureRV = mFX->mCache.LoadTexture("data/building4.dds", false, gd3dDevice);
			break;
		}
		mCube->SetOverrideMat(&mat);

		

		mLoadData.loadedSoFar++;

		FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(1, 1, 1), Vector3(0.15f, 0.15f, 0.15f), Vector3(0.25f, 0.25f, 0.25f));

	}
	//Turret and Skybox below here================================================================================================
	mQuad.Initialise(*mMeshMg->GetMesh("quad"));

	//textured lit box
	mBox.Initialise(*mMeshMg->GetMesh("cube"));
	mBox.GetPosition() = Vector3(0, -0.5f, 1);
	mBox.GetScale() = Vector3(0.5f, 0.5f, 0.5f);
	mat = mQuad.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.5, 0.5, 0.5, 1), Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("data/test.dds", false, gd3dDevice);
	mat.texture = "test.dds";
	mBox.SetOverrideMat(&mat);


	// floor
	mQuad.GetScale() = Vector3(3, 1, 3);
	mQuad.GetPosition() = Vector3(0, -1, 0);
	mat = mQuad.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("data/test.dds", false, gd3dDevice);
	mat.texture = "test.dds";
	mat.texTrsfm.scale = Vector2(10, 10);
	mQuad.SetOverrideMat(&mat);

	Mesh& sb = GetMeshManager()->CreateMesh("skybox");
	sb.CreateFrom("data/skybox.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mSkybox.Initialise(sb);
	mSkybox.GetScale() = Vector3(3, 3, 3);
	mSkybox.GetPosition() = Vector3(0, 0, 0);
	mSkybox.GetRotation() = Vector3(PI / 2, 0, 0);
	MaterialExt& defMat = mSkybox.GetMesh().GetSubMesh(0).material;
	defMat.flags &= ~MaterialExt::LIT;
	defMat.flags &= ~MaterialExt::ZTEST;

	Mesh& tm = GetMeshManager()->CreateMesh("3rdgun");
	tm.CreateFrom("data/Turret.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	thrdmGun.Initialise(tm);
	//mGun = mBox;
	thrdmGun.GetScale() = Vector3(0.01, 0.01, 0.01);
	thrdmGun.GetPosition() = Vector3(0, 0, 0);
	thrdmGun.GetRotation() = Vector3(0, XMConvertToRadians(180), 0);


	Mesh& fm = GetMeshManager()->CreateMesh("1stgun");
	fm.CreateFrom("data/Turret.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	fstmGun.Initialise(fm);
	//mGun = mBox;
	fstmGun.GetScale() = Vector3(0.01, 0.01, 0.01);
	fstmGun.GetPosition() = Vector3(0, -0.05, 0);
	fstmGun.GetRotation() = Vector3(0, XMConvertToRadians(180), 0);



	Mesh& lm = GetMeshManager()->CreateMesh("Lazer");
	lm.CreateFrom("data/Lazer.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mLaz.Initialise(lm);

	mLaz.GetScale() = Vector3(0.003, 0.003, 1);
	mLaz.GetPosition() = Vector3(0, 0.03f, 0);
	mLaz.GetRotation() = Vector3(0, XMConvertToRadians(180), 0);

	mFLaz = mLaz;
	mFLaz.GetPosition() = Vector3(0, -0.001, 0);
	mFLaz.GetScale() = Vector3(0.001, 0.001, 3);

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

void Game::Update(float dTime)
{
	if (mLoadData.running)
	{
		return;
	}

	mCamera.Update(dTime);
	gAngle += dTime * 0.5f;
	mBox.GetRotation().y = gAngle;
	mBox.GetPosition() = mCamera.GetPos() + Vector3(0, 0, 1);

	player.Update(dTime);
	player.UpdateCamera(dTime);

	if (GetMouseAndKeys()->GetMouseButton(MouseAndKeys::LBUTTON))
		if (mCamera.Shoot() != nullptr);

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
	ShowCursor(false);

	//screen resolution
	int sw, sh;
	GetClientExtents(sw, sh);
	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, sw / (sh*0.5f), 10, 100.f);
	Vector2 m = GetMouseAndKeys()->GetMouseMoveAndCentre();
	if (m.x != 0 || m.y != 0)
		mCamera.Rotate(dTime, m.x, m.y, 0);
	FX::GetViewMatrix() = gViewMat1;
	SetViewportDimensions(gd3dImmediateContext, 0, 0, sw, sh/2, gScreenViewport);
	FX::SetPerFrameConsts(gd3dImmediateContext, mCamera.GetPos());
	mSkybox.GetPosition() = mCamera.GetPos();
	FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);


	for (int i = 0; i < (int)mFlats.size(); ++i)
	{
		if (mFlats[i]->RetAlive())
			FX::GetMyFX()->Render(*mFlats[i], gd3dImmediateContext);
		mFlats[i]->ColiderUpdate();
	}

	FX::GetMyFX()->Render(*mQuad1, gd3dImmediateContext);
	FX::GetMyFX()->Render(*mQuad2, gd3dImmediateContext);
	//mpSpriteBatch->Begin(SpriteSortMode_Deferred, state.NonPremultiplied());
	//mpSpriteBatch->Draw(FX::GetMyFX() ->mCache.LoadTexture("test.dds", true, gd3dDevice), Vector2(sw / 2, sh / 4), nullptr, Colours::White, 0, Vector2(0, 0), Vector2(0.01f, 0.01f));
	//for (Model*p : mOpaques)
		//FX::GetMyFX()->Render(*p, gd3dImmediateContext);



	Matrix gm;
	fstmGun.GetWorldMatrix(gm);
	Matrix cam = mCamera.GetMatrix().Invert();
	gm = gm * cam;
	FX::GetMyFX()->Render(fstmGun, gd3dImmediateContext, nullptr, &gm);
	
	Matrix dm;
	mFLaz.GetWorldMatrix(dm);
	dm = dm * cam;
	
	if (!mCamera.CanFire())
		FX::GetMyFX()->Render(mFLaz, gd3dImmediateContext, nullptr, &dm);

	player.Render(FX::GetMyFX());
	
	player.GetCamera()->Render(FX::GetMyFX(), *player.GetPosition());
	FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);
	player.Render(FX::GetMyFX());

	for (int i = 0; i < (int)mFlats.size(); ++i)
	{
		if (mFlats[i]->RetAlive())
			FX::GetMyFX()->Render(*mFlats[i], gd3dImmediateContext);
		mFlats[i]->ColiderUpdate();
	}
	FX::GetMyFX()->Render(*mQuad1, gd3dImmediateContext);
	FX::GetMyFX()->Render(*mQuad2, gd3dImmediateContext);



	Matrix cm;
	thrdmGun.GetWorldMatrix(cm);
	cm = cm * cam;
	FX::GetMyFX()->Render(thrdmGun, gd3dImmediateContext, nullptr, &cm);


	mLaz.GetWorldMatrix(dm);
	dm = dm * cam;
	if(!mCamera.CanFire())
		FX::GetMyFX()->Render(mLaz, gd3dImmediateContext, nullptr, &dm);

	//mLaz.GetPosition() = mCamera.GetPos();
	


	EndRender();

//	GetMouseAndKeys()->PostProcess();
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


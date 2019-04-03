#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include <ctime>
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
/*
*/
Vector3 AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid) 
{
	/*AvoidQuad parameters explained
float y: the actual Y value at which you want to spawn
float minX, maxX: the minimum and maximum value for X in the TOTAL map quad
float minZ, maxZ:  the minimum and maximum value for Z in the TOTAL map quad
float minXAvoid, maxXAvoid: the minimum and maximum value for X in the AVOIDED quad
float minZAvoid, maxZAvoid:  the minimum and maximum value for Z in the AVOIDED quad

while loop keeps generating X and Z values until the point is not within the avoided quad

The vector 3 returned has float y parameter and an X and Z value outside the avoided quad
*/
	// This needs to know the previous Vector3 and include a check to avoid this Vector
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
	mQuad1->GetPosition() = Vector3(0, 0, 0);
	mQuad1->GetRotation() = Vector3(0, 0, 0);
	mQuad1->GetScale() = Vector3(3, 1, 3);
	mQuad1->SetOverrideMat(&mat);

	//avoided floor
	mQuad2->Initialise(BuildQuad(mMeshMgr));
	mat = mQuad2->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX.mCache.LoadTexture("building2.dds", true, gd3dDevice);
	mat.texture = "building2.dds";
	mQuad2->GetPosition() = Vector3(-1, 0.01f, -1);
	mQuad2->GetRotation() = Vector3(0, 0, 0);
	mQuad2->GetScale() = Vector3(1, 1, 1);
	mQuad2->SetOverrideMat(&mat);
	//*/





	//Skyscraper
	mCube = new Model_Kami();
	mCube->Initialise(BuildCube(mMeshMgr));
	mCube->GetScale() = Vector3(0.25f, 0.5f, 0.25f);
	mCube->GetPosition() = Vector3(0.0f, 0.5f, 0.5f);
	mat = mCube->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX.mCache.LoadTexture("building1.dds", true, gd3dDevice);
	mat.texture = "building1.dds"; //text label for debugging
	mCube->SetOverrideMat(&mat);
	//*/

	//Block of flats

	//ROB Block of flats
	///*
	SeedRandom(time(NULL));
	mFlats.clear();
	int w = (int)sqrt(mFlats.size());
	//mFlats.insert(mFlats.begin(), 100, mCube);
	for (size_t i = 0; i < 100; i++)
	{
		// Todo - Clear this later on
		auto* temp = new Model_Kami();
		temp->Initialise(mMeshMgr.GetMesh("box"));
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
		mFlats[i]->GetScale() = Vector3(0.1f, rY , 0.1f);
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
		mFlats[i]->GetPosition() = AvoidQuad(rY, xMin + 0.1f, xMax - 0.1f, zMin + 0.1f, zMax - 0.1f, minXAvoid -.5f, maxXAvoid + .5f, minZAvoid -.5f , maxZAvoid + .5f);
		mFlats[i]->GetRotation() = Vector3(0, GetRandom(0.f, 2 * PI), 0);
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
	mFX.Render(*mQuad1, gd3dImmediateContext);
	mFX.Render(*mQuad2, gd3dImmediateContext);
	mFX.Render(*mCube, gd3dImmediateContext); // Skyscraper
	for (int i = 0; i < (int)mFlats.size(); ++i) //Flats
		mFX.Render(*mFlats[i], gd3dImmediateContext);


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


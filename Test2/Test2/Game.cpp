#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include <ctime>
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
/*
*/
Vector3 AvoidQuad(float y, Vector4 generalQuad, Vector4 avoidQuad, Vector2 previousLocation)
{
	float& minX = generalQuad.x; //the minimum value for X in the TOTAL map quad
	float& maxX = generalQuad.y; //the maximum value for X in the TOTAL map quad
	float& minZ = generalQuad.z; //the minimum value for Z in the TOTAL map quad
	float& maxZ = generalQuad.w; //the maximum value for Z in the TOTAL map quad
	float& minXAvoid = avoidQuad.x; //minimum value for X in the AVOIDED quad
	float& maxXAvoid = avoidQuad.y; //maximum value for X in the AVOIDED quad
	float& minZAvoid = avoidQuad.z; //minimum value for Z in the AVOIDED quad
	float& maxZAvoid = avoidQuad.w; //maximum value for Z in the AVOIDED quad
	float& previousX = previousLocation.x; //X value for previous object's location
	float& previousZ = previousLocation.y; //Z value for previous object's location
	float& returnX = generalQuad.w; //Return X value initially set within generalQuad
	float& returnZ = generalQuad.y; //Return Z value initially set within generalQuad
	float xDifference = 0.f; //Difference between returnX and previousX
	float zDifference = 0.f; //Difference between returnZ and previousZ
	float xMaxDifference = 1; //Minimum X distance from previous location
	float zMaxDifference = 1; //Minimum Z distance from previous location
	while (((returnX < maxXAvoid && returnX > minXAvoid) && (returnZ < maxZAvoid && returnZ > minZAvoid)) || (((xDifference < xMaxDifference) && (xDifference > -xMaxDifference)) && ((zDifference < zMaxDifference) && (zDifference > -zMaxDifference))))
	{
		//

		//while (point is within avoidQuad) OR (point is within 0.1 radius of previous)

		//Above while conditions in more detail
		//(returnX is within avoidQuad) AND (returnZ is within avoidQuad)
		//OR
		//(xDifference is less than 0.1) AND (xDifference is more than -0.1)
		//AND
		//(zDifference is less than 0.1) AND (zDifference is more than -0.1)
		returnX = GetRandom(minX, maxX);
		xDifference = returnX - previousX;
		returnZ = GetRandom(minZ, maxZ);
		zDifference = returnZ - previousZ;
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
	mCube->GetPosition() = Vector3(0.f, 0.5f, -1.5f);
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
		auto temp = new Model_Kami();
		temp->Initialise(mMeshMgr.GetMesh("box"));
		temp->SetOverrideMat(&mat);
		mFlats.push_back(temp);
	}
	float xMin = -3;
	float xMax = 3;
	float zMin = -3;
	float zMax = 3;
	float minXAvoid = -1.5;
	float maxXAvoid = 0.5;
	float minZAvoid = -1.5;
	float maxZAvoid = 0.5;

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



	Vector4 avoidedQuad = Vector4(minXAvoid, maxXAvoid, minZAvoid, maxZAvoid);
	Vector4 generalQuad = Vector4(xMin, xMax, zMin, zMax);
	Vector2 previousPosition;
	for (int i = 0; i < 100; i++)
	{
		float rY = GetRandom(0.2f, 0.3f);
		mFlats[i]->GetScale() = Vector3(0.1f, rY, 0.1f);
		float mFlatX = -0.5;
		float mFlatZ = -0.5;
		float xDifference = 0;
		float xMinDifference = 0.1;
		float zDifference = 0;
		float zMinDifference = 0.1;
		if (i == 0)
		{
			previousPosition = Vector2(0, 0);
		}
		else
		{
			previousPosition = Vector2(mFlats[i - 1]->GetPosition().x, mFlats[i - 1]->GetPosition().y);
		}
		mFlats[i]->GetPosition() = AvoidQuad(rY, generalQuad, avoidedQuad, previousPosition);
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


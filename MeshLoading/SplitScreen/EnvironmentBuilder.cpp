#include "EnvironmentBuilder.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

void EnvironmentBuilder::Initialise()
{
	mMeshMg = GetMeshManager();
	mFX = FX::GetMyFX();
	//mQuad = new Model();
	//mQuad->Initialise(*mMeshMg->GetMesh("quad"));
}

void EnvironmentBuilder::BuildFloor()
{
	
	//BuildQuad(*mMeshMg);
	
	//Load skyscraper
	Floor = new Model();
	//AvoidPlane = new Model();
	//main floor
	Floor->Initialise(*mMeshMg->GetMesh("quad"));
	MaterialExt mat = Floor->GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = mFX->mCache.LoadTexture("data/Map1.dds", false, gd3dDevice);
	mat.texture = "Map1.dds";
	Floor->GetPosition() = Vector3(0, 0, 0);
	Floor->GetRotation() = Vector3(0, 0, 0);
	Floor->GetScale() = Vector3(3, 1, 3);
	Floor->SetOverrideMat(&mat);


}

//void EnvironmentBuilder::BuildAvoidedFloor()
//{
//
//	mQuad2 = new Model();
//
//	mQuad2->Initialise(*mMeshMg->GetMesh("quad"));
//	MaterialExt mat = mQuad2->GetMesh().GetSubMesh(0).material;
//	mat.pTextureRV = mFX->mCache.LoadTexture("../bin/data/building2.dds", false, gd3dDevice);
//	mat.texture = "building2.dds";
//	mQuad2->GetPosition() = Vector3(-1, 0.01f, -1);
//	mQuad2->GetRotation() = Vector3(0, 0, 0);
//	mQuad2->GetScale() = Vector3(1, 1, 1);
//	mQuad2->SetOverrideMat(&mat);
//
//}

void EnvironmentBuilder::BuildSkyscraper()
{

	mCube = new Model();
	mCube->Initialise(*mMeshMg->GetMesh("cube"));
	mCube->GetScale() = Vector3(0.25f, 0.5f, 0.25f);
	mCube->GetPosition() = Vector3(0.0f, 0.5f, 0.5f);
	//*/


	MaterialExt mat = mCube->GetMesh().GetSubMesh(0).material;
	mat.pTextureRV = mFX->mCache.LoadTexture("data/building1.dds", false, gd3dDevice);
	mat.texture = "building1.dds"; //text label for debugging
	mCube->SetOverrideMat(&mat);

}

void EnvironmentBuilder::BuildSkyBox()
{
	//SkyBox
	mSkybox = new Model();
	Mesh& sb = GetMeshManager()->CreateMesh("skybox");
	sb.CreateFrom("data/skybox.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mSkybox->Initialise(sb);
	mSkybox->GetScale() = Vector3(3, 3, 3);
	mSkybox->GetPosition() = Vector3(0, 0, 0);
	mSkybox->GetRotation() = Vector3(PI / 2, 0, 0);
	MaterialExt& defMat = mSkybox->GetMesh().GetSubMesh(0).material;
	defMat.flags &= ~MaterialExt::LIT;
	defMat.flags &= ~MaterialExt::ZTEST;
}

//void EnvironmentBuilder::BuildTrees()
//{
//	MaterialExt mat;
//	SeedRandom(time(NULL));
//	mFlats.clear();
//	int w = (int)sqrt(mFlats.size());
//	//mFlats.insert(mFlats.begin(), 100, mCube);
//	for (size_t i = 0; i < 100; i++)
//	{
//		// Todo - Clear this later on
//		Model* temp = new Model();
//		temp->Initialise(*mMeshMg->GetMesh("cube"));
//		temp->SetOverrideMat(&mat);
//		mFlats.push_back(temp);
//		//delete temp;
//		//temp = nullptr;
//	}
//	float xMin = -3;
//	float xMax = 3;
//	float zMin = -3;
//	float zMax = 3;
//	float minXAvoid = -2;
//	float maxXAvoid = 0;
//	float minZAvoid = -2;
//	float maxZAvoid = 0;
//
//	for (int i = 0; i < 100; i++)
//	{
//		float rY = GetRandom(0.2f, 0.3f);
//		mFlats[i]->GetScale() = Vector3(0.1f, rY, 0.1f);
//		float mFlatX = -0.5;
//		float mFlatZ = -0.5;
//		while ((mFlatX < 0 && mFlatX > -2) && (mFlatZ < 0 && mFlatZ > -2))
//		{
//			//if (i = 0)
//			//{
//			mFlatX = GetRandom(xMin, xMax);
//			mFlatZ = GetRandom(zMin, zMax);
//			//}
//			//else if (i >= 1 && (mFlatX != mFlats[i - 1]->GetPosition().x && mFlatZ != mFlats[i - 1]->GetPosition().z))
//			//{
//			//mFlatX = GetRandom(xMin, xMax);
//			//mFlatZ = GetRandom(zMin, zMax);
//			//}
//		}
//		mFlats[i]->GetPosition() = AvoidQuad(rY, xMin + 0.1f, xMax - 0.1f, zMin + 0.1f, zMax - 0.1f, minXAvoid - .5f, maxXAvoid + .5f, minZAvoid - .5f, maxZAvoid + .5f);
//		mFlats[i]->GetRotation() = Vector3(0, GetRandom(0.f, 2 * PI), 0);
//		//mFlats[i]->GetScale() = Vector3(0.1f, rY, 0.1f);
//		switch (GetRandom(0, 3)) {
//		case 0:
//			mat.pTextureRV = mFX->mCache.LoadTexture("data/building1.dds", false, gd3dDevice);
//			break;
//		case 1:
//			mat.pTextureRV = mFX->mCache.LoadTexture("data/building2.dds", false, gd3dDevice);
//			break;
//		case 2:
//			mat.pTextureRV = mFX->mCache.LoadTexture("data/building3.dds", false, gd3dDevice);
//			break;
//		case 3:
//			mat.pTextureRV = mFX->mCache.LoadTexture("data/building4.dds", false, gd3dDevice);
//			break;
//		}
//		mCube->SetOverrideMat(&mat);
//
//	}
//}

void EnvironmentBuilder::BuildObstacles()
{
	
}

void EnvironmentBuilder::RenderEnvironment()
{
	ResetStatesAfterSprites();
	mFX->Render(*mSkybox, gd3dImmediateContext);
	mFX->Render(*Floor, gd3dImmediateContext);
	

}
Vector3 EnvironmentBuilder::AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid) {
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


//Uses image reader to get track layout (obstacles, walls etc...)



void EnvironmentBuilder::Release()
{

	//delete mQuad;
	delete mCube;
	delete mSkybox;
	delete Floor;

}

Model* EnvironmentBuilder::getCube()
{
	return mCube;
}

void EnvironmentBuilder::UpdateSkyBox(Vector3 pos)
{
	mSkybox->GetPosition() = pos;
}
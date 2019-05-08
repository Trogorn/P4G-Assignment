#ifndef ENVIROH
#define ENVIROH

//Windows Library stuff that DirectX needs
#include <vector>
#include <future>
#include <sstream>
#include <thread>
#include <ctime>

//Mesh and Model Stuff
#include "Mesh.h"
#include "Model.h"
#include "GeometryBuilder.h"

//Direct X Stuff
#include "WindowUtils.h"
#include "D3D.h"
#include "GeometryBuilder.h"
#include "CommonStates.h"

#include "FX.h"
#include "SpriteFont.h"




class EnvironmentBuilder : public Singleton<EnvironmentBuilder>
{

public:
	EnvironmentBuilder() {};
	void Initialise();
	void BuildFloor();
	void BuildAvoidedFloor();
	void BuildSkyscraper();
	void BuildTrees();
	void BuildObstacles();
	void RenderEnvironment();
	void RenderSkyBox() {	mFX->Render(*mSkybox, gd3dImmediateContext); }
	void BuildSkyBox();
	void UpdateSkyBox(DirectX::SimpleMath::Vector3 pos);

	void Release();

	Model* getCube();
	Model* getQuad() { return mQuad; }

	MeshManager *mMeshMg;
	Model *mQuad, *mSkybox, *mCube, *Floor, *AvoidPlane;
	FX::MyFX* mFX;



	

protected:

private:

	DirectX::SimpleMath::Vector3 AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid);
};
SINGLETON_GET(EnvironmentBuilder);
#endif
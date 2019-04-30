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

	MeshManager *mMeshMgr;
	Model *mQuad1, *mQuad2, *mCube, *mTent, *mQuadPyramid, *mLight, *mCar;
	std::vector<Model*> mFlats;
	FX::MyFX* mFX;

protected:

private:



};
SINGLETON_GET(EnvironmentBuilder);
#endif
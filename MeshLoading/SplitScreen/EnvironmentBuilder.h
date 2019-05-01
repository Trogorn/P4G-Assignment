#pragma once

//Windows Library stuff that DirectX needs
#include <vector>
#include <future>
#include <sstream>
#include <thread>
#include <ctime>

//Mesh and Model Stuff
#include "Mesh.h"
#include "Model_Kami.h"
#include "GeometryBuilder.h"

//Direct X Stuff
#include "WindowUtils.h"
#include "D3D.h"
#include "GeometryBuilder.h"
#include "CommonStates.h"

#include "FX.h"
#include "SpriteFont.h"


class EnvironmentBuilder
{

public:

	EnvironmentBuilder() {};
	
	MeshManager mMeshMgr;
	Model_Kami *mQuad1, *mQuad2, *mCube, *mTent, *mQuadPyramid, *mLight, mCar;
	std::vector<Model_Kami*> mFlats;
	FX::MyFX mFX;

	void BuildFloor();
	void BuildAvoidedFloor();
	void BuildSkyscraper();
	void BuildTrees();
	void BuildObstacles();
	void RenderEnvironment();

protected:

private:



};

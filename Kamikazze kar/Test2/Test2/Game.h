#ifndef GAME_H
#define GAME_H

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

//Player Input
#include "Input.h"

//Character Stuff
#include "Turret.h"
#include "PlayerCar.h"

//Environment
#include "EnvironmentBuilder.h"

//Util Stuff
#include "MyDebug.h"




class Game
{
public:
	//start up and shut down
	Game() {}
	~Game() {
		Release();
	}
	//game logic, called constantly, elapsed time passed in
	void Update(float dTime);
	//render images, called constantly, elapsed time passed in
	void Render(float dTime);
	//called when ALT+ENTER or drag
	void OnResize(int screenWidth, int screenHeight);
	void Initialise();
	void Release();
	//message handling
	LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//This can go
	const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);


	MeshManager mMeshMgr;
	Model_Kami *mQuad1, *mQuad2, *mCube, *mTent, *mQuadPyramid, *mLight, mCar;
	std::vector<Model_Kami*> mFlats;
	FX::MyFX mFX;




private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;

	//load meshes
	void Load();
	//display a message
	void LoadDisplay(float dTime);
	//light position
	DirectX::SimpleMath::Vector3 mLightPos = DirectX::SimpleMath::Vector3(0, 0, 0);
	//text
	DirectX::SpriteBatch *mpSpriteBatch = nullptr;
	DirectX::SpriteFont *mpFont = nullptr, *mpFont2 = nullptr;

	//loading handler
	struct LoadData
	{
		//second thread
		std::future<void> loader;
		//how many meshes to load
		int totalToLoad = 0;
		//tally
		int loadedSoFar = 0;
		//is the loader active
		bool running = false;
	};
	LoadData mLoadData;

	float gAngle = 0;

	MouseAndKeys mMKInput;
	Gamepads mGamepad;
	Turret mCamera;

	PlayerCar player;

	EnvironmentBuilder BobTBuilder;


};

#endif

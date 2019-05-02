#pragma once
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <future>
#include <sstream>
#include <thread>
#include <ctime>
#include "Turret.h"
#include "Mesh.h"
#include "Model.h"
#include "FPSCamera.h"
#include "PlayerCar.h"
//#include "Input.h"
#include "MyDebug.h"
#include "SpriteFont.h"



class Game
{
public:

	Game()
	{

	}
	~Game() {
		Release();
	}
	void Update(float dTime);
	void Render(float dTime);
	void OnResize(int screenWidth, int screenHeight);
	void Initialise();
	void Release();
	LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	std::vector<Model*> mFlats;
	Model mQuad, mSkybox, *ThirdTurret, *FirstTurret, * mCar, * mLaz, * mFLaz, * mCube, * Floor, * AvoidPlane;
	MeshManager* mMeshMg;
	FX::MyFX* mFX;

	void LoadMap();


private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;
	float gAngle = 0;
	//FPSCamera mCamera2;
	std::vector<Model*> mOpaques;
	Turret PlayerTurret;
	PlayerCar PlayerDrive;

	float timer;
	//MouseAndKeys mMKInput;
	//Gamepads mGamepad;

	//load meshes
	void Load();

	void LoadLevel();

	//display a message
	void StartupDisplay(float dTime);

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

	Vector3 AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid);

	enum class State
	{
		MENU,
		PLAYING,
		ENDSCREEN
	};
	State gameState;




};

#endif


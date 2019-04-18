#pragma once
#ifndef GAME_H
#define GAME_H

#include <vector>
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

	Game() {}
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
	Model mBox, mQuad, mSkybox, mGun, mCar;

private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;
	float gAngle = 0;
	//FPSCamera mCamera2;
	std::vector<Model*> mOpaques;
	Turret mCamera;
	PlayerCar player;
	//MouseAndKeys mMKInput;
	//Gamepads mGamepad;
};

#endif


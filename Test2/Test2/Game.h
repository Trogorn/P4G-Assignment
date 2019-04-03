#ifndef GAME_H
#define GAME_H

#include <vector>

#include "Mesh.h"
#include "GeometryBuilder.h"
#include "FX.h"
#include "Model_Kami.h"
#include "Turret.h"

#include <vector>
#include <future>

#include "FX.h"
#include "SpriteFont.h"
#include "Input.h"




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

	const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	MeshManager mMeshMgr;
	Model_Kami *mQuad1, *mQuad2, *mCube, *mTent, *mQuadPyramid, *mLight;
	std::vector<Model_Kami*> mFlats;
	FX::MyFX mFX;




private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;

	float gAngle = 0;

	DirectX::SimpleMath::Vector3 mLightPos;
	
	MouseAndKeys mMKInput;
	Gamepads mGamepad;
	Turret mCamera;


};

#endif

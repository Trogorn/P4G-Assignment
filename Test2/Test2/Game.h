#ifndef GAME_H
#define GAME_H

#include <vector>

#include "Mesh.h"
#include "Model.h"
#include "FX.h"

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
	Vector3 AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid);
	LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	const DirectX::SimpleMath::Vector3 mDefCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	DirectX::SimpleMath::Vector3 mCamPos = DirectX::SimpleMath::Vector3(0, 2, -5);
	MeshManager mMeshMgr;
	Model mQuad1, mQuad2, mCube, mTent, mQuadPyramid, mLight;
	std::vector<Model> mFlats;

	FX::MyFX mFX;

private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;

	float gAngle = 0;

	DirectX::SimpleMath::Vector3 mLightPos;
};

#endif

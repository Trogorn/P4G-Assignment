#ifndef BALLSIM_H
#define BALLSIM_H

#include "Mesh.h"
#include "Model.h"
#include "Input.h"
#include "FX.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

class BallSim
{
public:
	Model mBall;
	Model mCube;
	DirectX::SimpleMath::Vector3 mVel = DirectX::SimpleMath::Vector3(0, 0, 0);
	DirectX::SimpleMath::Vector3 mGrav = DirectX::SimpleMath::Vector3(0, 0, 0);
	DirectX::SimpleMath::Vector3 mAccel = DirectX::SimpleMath::Vector3(0, 0, 0);
	float mCOR = 1;
	float mRadius = 0.1f;
	typedef enum { WAITING, LINEAR, LINEAR_ACCEL, BOUNCE_INF, BOUNCE, CUBE, CUBE_MOUSE } Mode;
	Mode mMode, mLastMode;

	void Initialise(MeshManager& mgr);
	void Input(char key);
	void Start();

	void Update(float dTime, const DirectX::SimpleMath::Vector3& camPos, MouseAndKeys& input, Model& rock);
	void Render(FX::MyFX& fx, float dTime);
	void RenderText(DirectX::SpriteFont *pF, DirectX::SpriteBatch *pBatch);
};

#endif

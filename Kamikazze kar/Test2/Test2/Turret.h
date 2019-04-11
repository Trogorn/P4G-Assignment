#pragma once
#include "d3d11.h"
#include "SimpleMath.h"
#include <vector>
#include "D3D.h"
#include "D3DUtil.h"
#include "Turret.h"
#include "Model_Kami.h"
class Turret
{
public:
	enum { UNLOCK = 999999 };

	
	void Initialise(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& tgt, DirectX::SimpleMath::Matrix& viewSpaceTfm);

	void SetPosition(DirectX::SimpleMath::Vector3);

	void Move(float dTime, bool forward, bool back, bool left, bool right);
	
	void Rotate(float dTime, float _yaw, float _pitch, float _roll);
	//stop camera moving in a certain axis, e.g. a FPS camera that always stays a fixed height in Y
	void LockMovementAxis(float x = UNLOCK, float y = UNLOCK, float z = UNLOCK) {
		mLockAxis = DirectX::SimpleMath::Vector3(x, y, z);
	}
	const DirectX::SimpleMath::Vector3& GetPos() {
		return mCamPos;
	}
	Model_Kami* Shoot();

	DirectX::SimpleMath::Vector3 GetShort();

	DirectX::SimpleMath::Vector3 GetRot();

	void Funcy(std::vector<Model_Kami*>& myvec)
	{
		mpMyVec = &myvec;
	}
private:
	
	std::vector<Model_Kami*> *mpMyVec;
	float shortestdistance;
	DirectX::SimpleMath::Vector3 hitPosition;
	DirectX::SimpleMath::Vector3 origin; 
	DirectX::SimpleMath::Vector3 direction;
	float distance = 15;
	
	DirectX::SimpleMath::Vector3 mLockAxis{ UNLOCK,UNLOCK,UNLOCK };
	//the camera matrix to modify
	DirectX::SimpleMath::Matrix* mpViewSpaceTfm;
	//movement and rotation speed
	float speed = 10.f, rspeed = 1.f;
	//camera position
	DirectX::SimpleMath::Vector3 mCamPos;
	//camera rotation
	float yaw = 0, pitch = 0, roll = 0;
};


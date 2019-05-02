#pragma once
#include "d3d11.h"
#include "SimpleMath.h"
#include <vector>
#include "D3D.h"
#include "D3DUtil.h"
#include "Model.h"
#include "FX.h"
#include "Input.h"
#include "GameObject.h"
#include "MyUtils.h"

class Turret : public GameObject
{
public:
	enum { UNLOCK = 999999 };

	
	void Initialise(Model *FirstTurret, Model *ThirdTurret, Model *FirstLaser, Model *ThirdLaser, const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& tgt, DirectX::SimpleMath::Matrix& viewSpaceTfm);

	void OnCollide(Model* Other, float dTime);

	void Update(float dTime);
	void MenuUpdate(float dTime);
	void Reset();

	void UpdatePosition(DirectX::SimpleMath::Vector3);

	
	void Rotate(float dTime);

	//stop camera moving in a certain axis, e.g. a FPS camera that always stays a fixed height in Y
	void LockMovementAxis(float x = UNLOCK, float y = UNLOCK, float z = UNLOCK) {
		mLockAxis = DirectX::SimpleMath::Vector3(x, y, z);
	}
	const DirectX::SimpleMath::Vector3& GetPos() {
		return mCamPos;
	}
	void Shoot();

	DirectX::SimpleMath::Vector3 GetShort();

	void Funcy(std::vector<Model*>& myvec)
	{
		mpMyVec = &myvec;
	}


	const DirectX::SimpleMath::Matrix& GetMatrix() {
		return *mpViewSpaceTfm;
	}

	bool CanFire()
	{
		return (cooldown <= maxCooldown - 0.1f);
	}
	

	void Renderfirst();
	void Renderthird();
	
private:
	


	std::vector<Model*> *mpMyVec;
	float shortestdistrance;
	DirectX::SimpleMath::Vector3 hitPosition;
	DirectX::SimpleMath::Vector3 origin; 
	DirectX::SimpleMath::Vector3 direction;
	float distance = 15;
	float cooldown = 0;
	float maxCooldown = 0.5f;
	DirectX::SimpleMath::Vector3 mLockAxis{ UNLOCK,UNLOCK,UNLOCK };
	//the camera matrix to modify
	DirectX::SimpleMath::Matrix* mpViewSpaceTfm;
	//movement and rotation speed
	float speed = 10.f, rspeed = 0.044f;
	//camera position
	DirectX::SimpleMath::Vector3 mCamPos;
	//camera rotation
	float yaw = 0, pitch = 0, roll = 0;

	Model *FirstTurret, *ThirdTurret, *Firstlaser, *Thirdlaser;
};
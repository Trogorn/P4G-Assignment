#pragma once
#include "GameObject.h"
#include "MyUtils.h"
#include "MyDebug.h"
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;
class Car : public GameObject
{
public:
	Car();
	void Initialise(Model_Kami* mModel, float Acceleration_Const, float Friction_Const, float Mass, float Braking_Const, float Min_Radius, float Turning_Mod, float Min_Turning_Speed);
	void OnCollide(const GameObject* apOther);
	void Update(float dTime);
	virtual ~Car();

	Vector3* GetDirection() { return &direction; }

protected:

	// X = Turning
	// Y = Acceleration
	Vector2 controlVector;
	Vector3 vCross;
	float dot;
	float det;
	float rotationAngle;

	float speed;
	
private:

	//Local space vector
	Vector3 direction;

	
	float ACCELERATION_CONST;
	float FRICTION_CONST;
	float MASS;
	float BRAKING_CONST;
	float MIN_RADIUS;
	float TURNING_MOD;
	float MIN_TURNING_SPEED;
	float force;
	float radius;

	//void Update(float) overridden

	void UpdateMovement(float dTime);

	void UpdateGameObject(float dTime);

	void UpdateHealth(int amount);

	//Will be overridden by both playercar and rc car
	virtual void UpdateControlVector() = 0;
};


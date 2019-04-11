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
	void Initialise(Model_Kami* mModel, float turnSpeed, float drag, float acceleration, float brakingForce, float maxSpeed, float reverseSpeed);
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

	
	int health;
	float turnSpeed;
	float drag;
	float acceleration;
	float brakingForce;
	float maxSpeed;
	float reverseSpeed;

	//void Update(float) overridden

	void UpdateMovement(float dTime);

	void UpdateGameObject(float dTime);

	void UpdateHealth(int amount);

	//Will be overridden by both playercar and rc car
	virtual void UpdateControlVector() = 0;
};


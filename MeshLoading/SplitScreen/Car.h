#pragma once
#include "GameObject.h"
#include "MyUtils.h"
#include "MyDebug.h"
#include <DirectXMath.h>
#include "Model.h"

using namespace DirectX::SimpleMath;
class Car : public GameObject
{
public:
	Car();
	void Initialise(Model* mModel, float Acceleration_Const, float Friction_Const, float Mass, float Braking_Const, float Min_Radius, float Turning_Mod, float Min_Turning_Speed, const std::vector<Model*> * flats);
	void OnCollide(Model* apOther);
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
	int health;
	Model* Other = nullptr;

	const std::vector<Model*> * pflats;

	//void Update(float) overridden

	void UpdateMovement(float dTime);

	void UpdateHealth(int amount);

	Model* HasCollided();

	//Will be overridden by both playercar and rc car
	virtual void UpdateControlVector() = 0;

	DirectX::BoundingOrientedBox* Collider;
};


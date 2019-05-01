#pragma once
#include "Car.h"
#include "Input.h"
#include "Camera.h"
#include "GameObject.h"
#include "Turret.h"

class PlayerCar :
	public Car
{
public:
	//Convert these from constructors to initialise/setup function
	PlayerCar();
	~PlayerCar();
	void Initialise(MouseAndKeys* MKInput, Model* mModel, float Acceleration_Const, float Friction_Const, float Mass, float Braking_Const, float Min_Radius, float Turning_Mod, float Min_Turning_Speed, Turret* turret);

	void UpdateCamera(float dTime);

	void UpdateControlVector();

	void Debug();

	void Render(FX::MyFX* fx);

	Camera* GetCamera() { return &camera; }

private:
	//Camera Offset unit Vector
	Vector3 CameraOffSetV;
	//Length of Camera Offset Vector
	float CameraOffSetD;
	//Allowed distance from offset
	Vector3 CameraDistance;

	//Turret turret;
	MouseAndKeys* MKInput;
	Camera camera;
	Turret* turret;
};


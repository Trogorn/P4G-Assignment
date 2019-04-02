#pragma once
#include "Car.h"
#include "Input.h"
#include "Camera.h"
class PlayerCar :
	public Car
{
public:
	//Convert these from constructors to initialise/setup function
	PlayerCar();
	~PlayerCar();
	void Initialise(MouseAndKeys* MKInput, Gamepads* GamePad, Model* mModel, float turnSpeed, float drag, float acceleration);

	void UpdateCamera(float dTime);

	void UpdateControlVector();

	void Debug();

	void Render(FX::MyFX fx);

private:
	//Camera Offset unit Vector
	Vector3 CameraOffSetV;
	//Length of Camera Offset Vector
	float CameraOffSetD;
	//Allowed distance from offset
	Vector2 CameraDistance;

	//Turret turret;
	MouseAndKeys* MKInput;
	Gamepads* GamePad;
	Camera camera;
};


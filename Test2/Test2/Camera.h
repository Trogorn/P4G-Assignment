#pragma once
#include "D3D.h"
#include "SimpleMath.h"
#include "FX.h"
#include "MyUtils.h"
#include "GameObject.h"

using namespace DirectX::SimpleMath;
class Camera
{
public:
	Camera();
	void Initialise(float FOV, GameObject* object);
	//Getter Setter for position
	Vector3* GetCameraPosition() { return &position; }
	void LerpCamera(Vector3 EndPoint, float dTime);
	void Render(FX::MyFX* fx, Vector3 playerPos);

private:
	Vector3 position;
	float FOV;
	GameObject* object;
};


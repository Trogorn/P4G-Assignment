#include "Camera.h"



Camera::Camera()
{
}

void Camera::LerpCamera(Vector3 EndPoint, float dTime)
{
	position = Vector3::Lerp(position, EndPoint, dTime);
}

void Camera::Initialise(float FOV, GameObject* object)
{
	this->FOV = FOV;
	this->object = object;
}

void Camera::Render(FX::MyFX* fx, Vector3 playerPos)
{

	int ScreenWidth, ScreenHeight;
	GetClientExtents(ScreenWidth, ScreenHeight);

	float AspectRatio = ScreenWidth / (ScreenHeight * 0.5f);

	//View matrix of Player Camera
	Matrix PlayerCamera;

	//Create Camera Matrix
	CreateViewMatrix(PlayerCamera, position, Vector3(playerPos.x, playerPos.y + 0.5f, playerPos.z) , Vector3(0, 1, 0));
	
	FX::GetViewMatrix() = PlayerCamera;
	SetViewportDimensions(gd3dImmediateContext, 0, 1, ScreenWidth, ScreenHeight * 0.5f, gScreenViewport);
	FX::SetPerFrameConsts(gd3dImmediateContext, position);
	
	//Create Projection Matrix to be applied to the Camera
	CreateProjectionMatrix(FX::GetProjectionMatrix(), MyUtils::Deg2Rad(FOV), AspectRatio, 1, 1000.f);
}
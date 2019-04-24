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

	//Create Projection Matrix to be applied to the Camera
	CreateProjectionMatrix(FX::GetProjectionMatrix(), MyUtils::Deg2Rad(FOV), AspectRatio, 0.1f, 10.f);

	//Create Camera Matrix
	CreateViewMatrix(PlayerCamera, position, Vector3(playerPos.x, playerPos.y, playerPos.z) , Vector3(0, 1, 0));
	
	FX::GetViewMatrix() = PlayerCamera;
	//Set ViewportDimensions to be the bottom half of the screen
	//Also set hieght to only be half the screen
	SetViewportDimensions(gd3dImmediateContext, 0, ScreenHeight * 0.5f, ScreenWidth, ScreenHeight * 0.5f, gScreenViewport);
	FX::SetPerFrameConsts(gd3dImmediateContext, position);
}
	
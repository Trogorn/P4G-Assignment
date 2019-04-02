#include "PlayerCar.h"


PlayerCar::PlayerCar() :Car()
{
	camera.Initialise(103, this);
}

void PlayerCar::Initialise(MouseAndKeys* MKInput, Gamepads* GamePad, Model* mModel, float turnSpeed, float drag, float acceleration)
{
	this->MKInput = MKInput;
	this->GamePad = GamePad;
	Car::Initialise(mModel, turnSpeed, drag, acceleration);

	CameraOffSetV = Vector3(0.f, 1.f, -2.f);
	CameraOffSetD = 2.f;
	CameraDistance = Vector2(3.f, 4.f);
}


PlayerCar::~PlayerCar()
{

}

//Get Player input
void PlayerCar::UpdateControlVector()
{
	if (MKInput->IsPressed(VK_W) && MKInput->IsPressed(VK_S))
		controlVector.y = 0;
	else
	{
		if (MKInput->IsPressed(VK_W))
		{
			controlVector.y = 1;
		}
		else
		{
			if (MKInput->IsPressed(VK_S))
			{
				controlVector.y = -1;
			}
			else
			{
				controlVector.y = 0;
			}
		}
	}
	if (MKInput->IsPressed(VK_A) && MKInput->IsPressed(VK_D))
		controlVector.x = 0;
	else
	{
		if (MKInput->IsPressed(VK_A))
			controlVector.x = -1;
		else
		{
			if (MKInput->IsPressed(VK_D))
				controlVector.x = 1;
			else
				controlVector.x = 0;
		}
	}

}

void PlayerCar::UpdateCamera(float dTime)
{
	//Model has a world matrix meaning I can work with local space and then convert it into world coordinates later

	// Local to World
	Matrix L2W = GetWorldMatrix();
	// World to Local
	Matrix W2L = L2W.Invert();

	//Local Offset Position
	Vector3 OffsetL = CameraOffSetV * CameraOffSetD;

	//World position of CameraOffset
	Vector3 OffsetW = Vector3::Transform(OffsetL, L2W);

	//Lerp Camera in world space
	camera.LerpCamera(OffsetW, dTime);

	//Convert cameraPos to local space
	Vector3 cameraPosL = Vector3::Transform(*camera.GetCameraPosition(), W2L);

	//Vector from Offset(Local) -> CameraPos(Local)
	Vector3 Offset2Camera = cameraPosL - OffsetL;


	// Check distance X (local)
	if (Offset2Camera.x > CameraDistance.x)
	{
		cameraPosL.x = CameraDistance.x;
	}
	else
		if (Offset2Camera.x < -CameraDistance.x)
		{
			cameraPosL.x = -CameraDistance.x;
		}

	// Check distance Z (local)
	if (Offset2Camera.z > CameraDistance.y)
	{
		cameraPosL.z = CameraDistance.y;
	}
	else
		if (-Offset2Camera.z < -CameraDistance.y)
		{
			cameraPosL.z = -CameraDistance.y;
		}


	*camera.GetCameraPosition() = Vector3::Transform(cameraPosL, L2W);
	


	////Update Location of CameraOffSet point
	//Matrix m = Matrix::CreateRotationY(Car::rotationAngle);
	//Vector3::TransformNormal(CameraOffSetV, m, CameraOffSetV);
	//CameraOffSetV.Normalize();

	//Vector3 OffsetPoint = *GetPosition() + (CameraOffSetV * CameraOffSetD);
	//
	////Lerp the camera towards point
	//camera.LerpCamera(OffsetPoint, dTime);

	////Now calc distance
	//
	//Vector3 direction = *camera.GetCameraPosition() - OffsetPoint;
	//float distance = direction.Length();
	//direction.Normalize();

	////Test Distance
	//if (distance > CameraDistance.x)
	//{
	//	//Set camera pos
	//	*camera.GetCameraPosition() = OffsetPoint + direction*CameraDistance.x;
	//}

	//Maybe split this into relative X distance and Z distance for more customisation
	//Although I have a feeling this will be a bitch

	//Calculate Local Z distance

	//Calculate Local X Distance
}

void PlayerCar::Debug()
{
	std::string ss;
	ss = "";
	//ss += "Direction X: ";
	//ss += std::to_string(GetDirection()->x);
	//ss += "\nDirection Y: ";
	//ss += std::to_string(GetDirection()->y);
	//ss += "\nDirection Z: ";
	//ss += std::to_string(GetDirection()->z);
	//ss += "\nRotation X: ";
	//ss += std::to_string(GetRotation()->x);
	//ss += "\nRotation Y: ";
	//ss += std::to_string(GetRotation()->y);
	//ss += "\nRotation Z: ";
	//ss += std::to_string(GetRotation()->z);
	//ss += "\nForward X: ";
	//ss += std::to_string(GetForward()->x);
	//ss += "\nForward Y: ";
	//ss += std::to_string(GetForward()->y);
	//ss += "\nForward Z: ";
	//ss += std::to_string(GetForward()->z);
	ss += "\nSpeed: ";
	ss += std::to_string(speed);
	MyDebug::Message(ss);
}

void PlayerCar::Render(FX::MyFX fx)
{
	GameObject::Render(fx);
	camera.Render(fx, *GetPosition());
}
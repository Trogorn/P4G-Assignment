#include "PlayerCar.h"


PlayerCar::PlayerCar() :Car()
{
	camera.Initialise(103, this);
}

//Some things don't work with the scale needs to be fixed
void PlayerCar::Initialise(MouseAndKeys* MKInput, Gamepads* GamePad, Model_Kami* mModel, float turnSpeed, float drag, float acceleration, float brakingForce, float maxSpeed, float reverseSpeed, Turret* turret)
{
	this->turret = turret;
	this->MKInput = MKInput;
	this->GamePad = GamePad;
	Car::Initialise(mModel, turnSpeed, drag, acceleration, brakingForce, maxSpeed, reverseSpeed);

	CameraOffSetV = Vector3(0.f, 1.f, -2.f);
	CameraOffSetD = 2.f;
	CameraDistance = Vector3(3.f, 4.f, 0.f);
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
	if (MKInput->IsPressed(VK_SPACE))
	{
		speed = 0;
	}

}

//This breaks if the car reverses past the camera
// Look into this
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
		cameraPosL.x = OffsetL.x + CameraDistance.x;
	}
	else
	{
		if (Offset2Camera.x < -CameraDistance.x)
		{
			cameraPosL.x = OffsetL.x + -CameraDistance.x;
		}
	}

	//Check distance Z (local, negative = backwards from front of car)
	if (Offset2Camera.z < -CameraDistance.y)
	{
		cameraPosL.z = OffsetL.z + -CameraDistance.y;
	}
	else
	{
		//Check distance Z (local, positive = towards front of car)
		if (Offset2Camera.z > CameraDistance.y/2)
		{
			cameraPosL.z = OffsetL.z + CameraDistance.y/2;
		}
	}


	MyDebug::Message(std::to_string(Offset2Camera.z));
	MyDebug::Message(std::to_string(CameraDistance.y));
	MyDebug::Message(std::to_string(cameraPosL.z));


	//Convert Back to world coordinates and send to the Camera
	*camera.GetCameraPosition() = Vector3::Transform(cameraPosL, L2W);


	//turret->SetPosition( Vector3::Transform(Vector3(0,0.25f,0), GetWorldMatrix()) );

	turret->SetPosition(Vector3(0, 0.15f, 0) + *GetPosition());
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

void PlayerCar::Render(FX::MyFX* fx)
{
	GameObject::Render(fx);
	//camera.Render(fx, *GetPosition());
}
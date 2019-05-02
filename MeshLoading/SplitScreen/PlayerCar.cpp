#include "PlayerCar.h"


PlayerCar::PlayerCar() :Car()
{
	camera.Initialise(103, this);
}

//MKInput = Mouse/Keyboard Input
//Gamepad = Gamepad Input
//mModel = Model of playerCar
//Acceleration_Const = Force Accelerating the Car
//Friction_Const = Percentage of Current speed Car loses per frame
//Mass = Mass of the car in KiloGrams
//Braking_Const = Force Braking the Car
//Min_Radius = The smallest turning radius the car can turn
//Turning_Mod = How much the speed affects the turning radius of the car
//Min_Turning_Speed = Minium Speed the car must be travelling in order to turn (usually 1)

void PlayerCar::Initialise(MouseAndKeys* MKInput, Model* mModel, float Acceleration_Const, float Friction_Const, float Mass, float Braking_Const, float Min_Radius, float Turning_Mod, float Min_Turning_Speed, Turret* turret, const std::vector<Model*> * flats)
{
	this->turret = turret;
	this->MKInput = MKInput;
	//this->GamePad = GamePad;
	Car::Initialise(mModel, Acceleration_Const, Friction_Const, Mass, Braking_Const, Min_Radius, Turning_Mod, Min_Turning_Speed, flats);

	CameraOffSetVDefault = Vector3(0.f, 1.f, -2.f);
	CameraOffSetV = CameraOffSetVDefault;
	CameraOffSetD = 1.f;
	CameraDistance = Vector3(2.f, 2.f, 0.f);
}


PlayerCar::~PlayerCar()
{

}

//Get Player input
void PlayerCar::UpdateControlVector()
{
	if (MKInput->IsPressed(VK_W) && MKInput->IsPressed(VK_S))
		control.y = 0;
	else
	{
		if (MKInput->IsPressed(VK_W))
		{
			control.y = 1;
		}
		else
		{
			if (MKInput->IsPressed(VK_S))
			{
				control.y = -1;
			}
			else
			{
				control.y = 0;
			}
		}
	}
	if (MKInput->IsPressed(VK_A) && MKInput->IsPressed(VK_D))
		control.x = 0;
	else
	{
		if (MKInput->IsPressed(VK_A))
			control.x = -1;
		else
		{
			if (MKInput->IsPressed(VK_D))
				control.x = 1;
			else
				control.x = 0;
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

	if (speed < 0)
	{
		CameraOffSetV.z = CameraOffSetVDefault.z * -1;
	}
	else
	{
		CameraOffSetV.z = CameraOffSetVDefault.z;
	}
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
		if (Offset2Camera.z > CameraDistance.y / 2)
		{
			cameraPosL.z = OffsetL.z + CameraDistance.y / 2;
		}
	}


	//MyDebug::Message(std::to_string(Offset2Camera.z));
	//MyDebug::Message(std::to_string(CameraDistance.y));
	//MyDebug::Message(std::to_string(cameraPosL.z));


	//Convert Back to world coordinates and send to the Camera
	*camera.GetCameraPosition() = Vector3::Transform(cameraPosL, L2W);


	turret->UpdatePosition( Vector3::Transform(Vector3(0, 0.6f,-1.f), GetWorldMatrix()) );

	//turret->SetPosition(Vector3(0, 0.75f, -1.f));
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

void PlayerCar::Render()
{
	GameObject::Render();
	//camera.Render(fx, *GetPosition());
}

void PlayerCar::MenuUpdate(float dTime)
{
	*GetRotation() = Vector3(0, GetRotation()->y + dTime, 0);
	turret->UpdatePosition(Vector3::Transform(Vector3(0, 0.6f, -1.f), GetWorldMatrix()));
}

void PlayerCar::Reset()
{
	*GetRotation() = Vector3(0, 0, 0);
}
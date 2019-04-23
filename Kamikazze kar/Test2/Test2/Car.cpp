#include "Car.h"


Car::Car():GameObject()
{
}

void Car::Initialise(Model_Kami* mModel, float Acceleration_Const, float Friction_Const, float Mass, float Braking_Const, float Min_Radius, float Turning_Mod, float Min_Turning_Speed)
{
	ACCELERATION_CONST	=	Acceleration_Const;
	MASS				=	Mass;
	BRAKING_CONST		=	Braking_Const;
	MIN_RADIUS			=	Min_Radius;
	TURNING_MOD			=	Turning_Mod;
	MIN_TURNING_SPEED	=	Min_Turning_Speed;
	FRICTION_CONST		=	Friction_Const;
	speed = 0;
	force = 0;
	radius = 0;

	GameObject::Initialise(mModel);
}


void Car::OnCollide(const GameObject* apOther)
{
	
}

void Car::Update(float dTime)
{
	UpdateControlVector();
	UpdateMovement(dTime);
	UpdateGameObject(dTime);

}

void Car::UpdateControlVector()
{
	// Do nothing
	// Player car will update this from input
	// AI cars will update this from ai calculations
}


//Updated Movement function

void Car::UpdateMovement(float dTime)
{
	//Acceleration and Braking
	//========================================================================
	float throttle = controlVector.y;
	if (!(throttle < 0))
	{
		//If not braking
		//Work out force then add frictional force
		force = (ACCELERATION_CONST * throttle) - (speed * FRICTION_CONST);
	}
	else
	{
		//If braking
		//Work out force then add the frictional force
		force = (-BRAKING_CONST * abs(throttle)) - (speed * FRICTION_CONST);
	}
	
	speed += (force / MASS) * dTime;
	MyDebug::Message("Force: " + std::to_string(force));
	MyDebug::Message("Speed: " + std::to_string(speed));
	//=====================================================================

	//Turning Mechanics
	//=======================================================================
	float wheel = controlVector.x;

	//If Turning
	if (wheel != 0)
	{
		if (speed > MIN_TURNING_SPEED)
		{
			radius = MIN_RADIUS + (speed * TURNING_MOD);
			float CarAngle,radiusAngle,deltaAngle,newAngle;
			//Turn left
			if (wheel < 0)
			{
				//Get CarAngle
				CarAngle = GetRotation()->y;

				//Get the angle from Centre of cirle to car
				radiusAngle = CarAngle - 180;

				//Calculate Angle car travels through
				deltaAngle = (speed / (PI * (2 * radius)) * 360) * dTime;

				//Calculate Angle Car should be at after turning
				newAngle = radiusAngle - deltaAngle;
			}
			//Turn Right
			else
			{
				//Get CarAngle
				CarAngle = GetRotation()->y;
				//Get the angle from Centre of cirle to car
				radiusAngle = CarAngle;
				//Calculate Angle car travels through
				deltaAngle = (speed / (PI * (2 * radius)) * 360) * dTime;
				//Calculate Angle Car should be at after turning
				newAngle = radiusAngle + deltaAngle;
			}


			//Pos Relative to centre Pos
			Vector3 CarPos = Vector3::Zero;
			CarPos.z = radius * -cos(radiusAngle * MyUtils::Deg2Rad);
			CarPos.x = radius * sin(radiusAngle * MyUtils::Rad2Deg);
			CarPos.y = GetPosition()->y;

			/*MyDebug::Message("Car Pos X: " + std::to_string(CarPos.x));
			MyDebug::Message("Car Pos Y: " + std::to_string(CarPos.y));
			MyDebug::Message("Car Pos Z: " + std::to_string(CarPos.z));*/


			//Calculate Centre Position
			Vector3 centrePos = Vector3::Zero;
			centrePos = *GetPosition() - CarPos;
			centrePos.y = GetPosition()->y;

			//Calulcate Cars new Position
			Vector3 NewPos = Vector3::Zero;
			NewPos.z = radius * -cos(newAngle * MyUtils::Deg2Rad) + centrePos.x;
			NewPos.x = radius * sin(newAngle * MyUtils::Deg2Rad) + centrePos.z;
			NewPos.y = GetPosition()->y;

			*GetPosition() = NewPos;

			//Turn Left
			if (wheel < 0)
			{
				*GetRotation() = Vector3(0, GetRotation()->y - deltaAngle, 0);
			}
			//Turn Right
			else
			{
				*GetRotation() = Vector3(0, GetRotation()->y + deltaAngle, 0);
			}

			/*MyDebug::Message("Centre pos X: " + std::to_string(centrePos.x));
			MyDebug::Message("Centre pos Y: " + std::to_string(centrePos.y));
			MyDebug::Message("Centre pos Z: " + std::to_string(centrePos.z));*/
		}
	}
	//If Not Turning
	else
	{
		//This doesn't work right
		Vector3 Pos = Vector3::Zero;
		Pos.z = speed;
		MyDebug::Message("Pos Before" + std::to_string(Pos.z));
		Vector3::Transform(Pos,this->GetWorldMatrix(),Pos);
		MyDebug::Message("Pos After" + std::to_string(Pos.z));
	}
	//=======================================================================
}


void Car::UpdateGameObject(float dTime)
{

}

//Can be used to increase or decrease health
void Car::UpdateHealth(int amount)
{
}

Car::~Car()
{

}
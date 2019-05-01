#include "Car.h"


Car::Car():GameObject()
{
}

void Car::Initialise(Model* mModel, float Acceleration_Const, float Friction_Const, float Mass, float Braking_Const, float Min_Radius, float Turning_Mod, float Min_Turning_Speed, const std::vector<Model*> * flats)
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
	Collider = &mModel->GetCollider();
	pflats = flats;

	GameObject::Initialise(mModel);
}



void Car::Update(float dTime)
{
	GetModel()->ColiderUpdate();
	UpdateControlVector();
	UpdateMovement(dTime);
	Other = HasCollided();
	if (Other != nullptr)
	{
		OnCollide(Other);
	}
}

void Car::OnCollide(Model* Other)
{
	Other->Die();
	speed *= 0.25;
	UpdateHealth(-1);
	Other = nullptr;
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
	
	float acceleration = force / MASS;
	speed += acceleration * dTime;
	MyDebug::Message(speed);
	//=====================================================================

	//Turning Mechanics
	//=======================================================================
float wheel = controlVector.x;
//If Turning
if (wheel != 0)
{
	if (speed > MIN_TURNING_SPEED)
	{
		radius = (MIN_RADIUS + (speed * TURNING_MOD)) * GetScale()->x;
		float CarAngle, radiusAngle, deltaAngle, newAngle;
		Vector3 CarPos = Vector3::Zero;
		Vector3 NewPos = Vector3::Zero;
		//Turn left
		if (wheel < 0)
		{
			//Get CarAngle
			CarAngle = GetRotation()->y;

			//Get the angle from Centre of cirle to car
			radiusAngle = (CarAngle)* MyUtils::Rad2Deg;

			//Calculate Angle car travels through
			deltaAngle = ((speed / (PI * (2 * radius)) * 360) * dTime) * GetScale()->x;

			//Calculate Angle Car should be at after turning
			newAngle = radiusAngle + deltaAngle;


		}
		//Turn Right
		else
		{
			//Get CarAngle
			CarAngle = GetRotation()->y;
			//Get the angle from Centre of cirle to car
			radiusAngle = CarAngle * MyUtils::Rad2Deg;

			//Calculate Angle car travels through
			deltaAngle = (speed / (PI * (2 * radius)) * 360) * dTime * GetScale()->x;

			//Calculate Angle Car should be at after turning
			newAngle = radiusAngle + deltaAngle;

		}

		MyDebug::Message(deltaAngle);
		MyDebug::Message(newAngle);


		//Pos Relative to centre Pos

		CarPos.x = radius * -cos(radiusAngle * MyUtils::Deg2Rad);
		CarPos.z = radius * sin(radiusAngle * MyUtils::Deg2Rad);
		CarPos.y = GetPosition()->y;

		MyDebug::Message("Radius Angle: " + std::to_string(radiusAngle));
		MyDebug::Message("CarPos X: " + std::to_string(CarPos.x));

		//Calculate Centre Position
		Vector3 centrePos = Vector3::Zero;
		centrePos = *GetPosition() - CarPos;
		centrePos.y = GetPosition()->y;

		//Calulcate Cars new Position

		NewPos.x = radius * -cos(newAngle * MyUtils::Deg2Rad) + centrePos.x;
		NewPos.z = radius * sin(newAngle * MyUtils::Deg2Rad) + centrePos.z;
		NewPos.y = GetPosition()->y;

		*GetPosition() = NewPos;

		//Turn Left
		if (wheel < 0)
		{
			*GetRotation() = Vector3(0, GetRotation()->y - (deltaAngle* MyUtils::Deg2Rad), 0);
		}
		//Turn Right
		else
		{
			*GetRotation() = Vector3(0, GetRotation()->y + (deltaAngle* MyUtils::Deg2Rad), 0);
		}
	}
}
//If Not Turning
else
{
	Vector3 Pos = Vector3::Zero;
	Pos.z = speed * dTime;
	*GetPosition() = Vector3::Transform(Pos, this->GetWorldMatrix());
}
//=======================================================================
}

//Can be used to increase or decrease health
void Car::UpdateHealth(int amount)
{
	health += amount;
}

Model* Car::HasCollided()
{
	DirectX::BoundingOrientedBox* otherCollider;
	for (int i = pflats->size() - 1; i >= 0; i--)
	{
		otherCollider = &pflats->operator[](i)->GetCollider();
		if (Collider->Intersects(*otherCollider) && pflats->operator[](i)->GetAlive())
		{
			return pflats->operator[](i);
		}
	}
	return nullptr;
}


Car::~Car()
{

}
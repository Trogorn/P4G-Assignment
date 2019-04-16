#include "Car.h"


Car::Car():GameObject()
{
}

void Car::Initialise(Model_Kami* mModel, float turnSpeed, float drag, float acceleration, float brakingForce, float maxSpeed, float reverseSpeed)
{
	this->speed = speed;
	this->turnSpeed = turnSpeed;
	this->friction = drag;
	this->acceleration = acceleration;
	this->brakingForce = brakingForce;
	this->maxSpeed = maxSpeed;
	this->reverseSpeed = reverseSpeed;
	direction = direction.UnitZ;
	//GameObject* pGO = this;
	//pGO->Initialise(mModel);
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

	//LONGITUDE (forward/back)=============================================================

	//Friction

	friction = max(speed*friction_Constant, friction_Constant);

	//Accelerating force
	force = acceleration_force*controlVector.y - friction;

	//Acceleration
	acceleration = force / mass;

	//Speed
	speed += acceleration;

	//TURNING================================================================

	float radius = abs(speed * controlVector.y);

	//Turning left or right
	//Positive = right
	//Negative = left
	int sign;
	if (controlVector.y < 0)
	{
		sign = -1;
	}
	else
	{
		if (controlVector.y == 0)
		{
			sign = 0;
		}
		else
		{
			sign = 1;
		}
	}

	Vector3 centre;
	if (controlVector.y < 0)
	{
		centre.x += radius * sign;
	}

	// Get local position car is moving to

	Vector3 Position = Vector3::Zero;

	Position.x = radius*cos(dTime)*sign;
	Position.z = radius*sin(dTime);

	//Convert local postion to world position

	//Move car to world position

}

////This function updates speed value, and turn degrees/radians
//void Car::UpdateMovement(float dTime)
//{
//
//	//Redo this to make sure its all local
//
//	//ACCELERATION/DRAG---------------------------------------------------------------------------------------------------------------
//
//	//Control vector contains:
//	// y accel -1 = full brake/reverse  0 = coast  1 = full accel
//	// x turning -1 = full left  0 = no turn  1 = full right
//
//	//Speed should be a float indicating how fast the vehicle is moving
//
//	//If accelerating
//	if (controlVector.y > 0)
//	{
//		speed += acceleration * controlVector.y * dTime;
//	}
//	else
//	{
//		//If braking
//		if (controlVector.y < 0)
//		{
//			speed += brakingForce * controlVector.y * dTime;
//		}
//	}
//
//	//Limit max speed
//	if (speed > maxSpeed)
//	{
//		speed = maxSpeed;
//	}
//	else
//	{
//		if (speed < -reverseSpeed)
//		{
//			speed = -reverseSpeed;
//		}
//	}
//
//	//speed += (controlVector.y * acceleration) * dTime;
//	// maybe rework this to get something that feels nicer?
//
//
//	//Mess around with drag
//	if (speed > 0)
//	{
//		speed -= friction * dTime;
//	}
//	else
//		if (speed < 0)
//		{
//			speed += friction * dTime;
//		}
//
//
//	//Turning probably moved to updateworld object
//
//	////TURNING------------------------------------------------------------------------------------------------------------------------
//
//	////Convert turnspeed into rads and multiply by controlVector to get distance to rotate
//	//float turn = MyUtils::Deg2Rad(turnSpeed) * controlVector.x * dTime;
//
//	////Create Rotation matrix from radians
//	//Matrix m = Matrix::CreateRotationY(turn);
//
//	////Transform direction by rotation matrix
//	//direction = direction.TransformNormal(direction, m);
//
//	//// If direction is not currently a unit vector, normalize it
//	//if (direction.LengthSquared() != 1)
//	//{
//	//	direction.Normalize();
//	//}
//
//}


//This function updates the actual position and rotation of the model/object
//might merge this with Update Movement
void Car::UpdateGameObject(float dTime)
{
	//Update this to make sure its all local 2 World space

	//If working with local space speed would be a vector heading straight forward
	//The object would get rotated in world space, causing said speed vector to be rotated as well.

	//So each frame the order would be this

	//Update speed
	//Calculate change in direction?
	//Rotate object
	//Move object forward by speed


	//Update Speed s above in update movement

	// Next thing is to calculate turning

	// So using control vector work out how many degrees/radians the 'car' has turned by?
	float turn = MyUtils::Deg2Rad(turnSpeed) * controlVector.x * dTime; //This line should work for that

	// Rotate 'car' by this amount
	// Rotate car by turn
	//Car shouldn't be able to turn on the spot now
	// Maybe link the more to speed like
	// += turn / speed?
	// and if speed < x turn doesn't work?
	if (speed != 0)
	{
		GetRotation()->y += turn;
	}

	// Move 'car' 'forward'
	*GetPosition() = Vector3::Transform(speed * Vector3::UnitZ, GetWorldMatrix());
}

//Can be used to increase or decrease health
void Car::UpdateHealth(int amount)
{
	health += amount;
}

Car::~Car()
{

}
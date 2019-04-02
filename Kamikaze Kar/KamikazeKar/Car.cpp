#include "Car.h"


Car::Car():GameObject()
{
}

void Car::Initialise(Model* mModel, float turnSpeed, float drag, float acceleration, float maxSpeed)
{
	this->speed = speed;
	this->turnSpeed = turnSpeed;
	this->drag = drag;
	this->acceleration = acceleration;
	this->maxSpeed = maxSpeed;
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
	UpdateGameObject();

}

void Car::UpdateControlVector()
{
	// Do nothing
	// Player car will update this from input
	// AI cars will update this from ai calculations
}

void Car::UpdateMovement(float dTime)
{

	//ACCELERATION/DRAG---------------------------------------------------------------------------------------------------------------

	//Control vector contains:
	// y accel -1 = full brake/reverse  0 = coast  1 = full accel
	// x turning -1 = full left  0 = no turn  1 = full right

	speed += (controlVector.y * acceleration) * dTime;
	// maybe rework this to get something that feels nicer?


	//Mess around with drag
	if (speed > 0)
	{
		speed -= drag * dTime;
	}
	else
		if (speed < 0)
		{
			speed += drag * dTime;
		}


	//TURNING------------------------------------------------------------------------------------------------------------------------

	//Alternative add a vector at 90 degrees to current then normalise?

	//Convert turnspeed into rads and multiply by controlVector to get distance to rotate
	float turn = MyUtils::Deg2Rad(turnSpeed) * controlVector.x * dTime;

	//Create Rotation matrix from radians
	Matrix m = Matrix::CreateRotationY(turn);

	//Transform direction by rotation matrix
	direction = direction.Transform(direction, m);

	// Make sure direction is a unit vector (Is this actually needed maybe check to see if this is needed since Normalizing is slow)
	direction.Normalize();

}

void Car::UpdateGameObject()
{
	//Get Reference to Position Vector
	Vector3* pPosition = GetPosition();

	//Pointer
	Vector3* pRotation = GetRotation();

	if (speed != 0)
		*pPosition += (speed * direction);

	//Rotation

	//Car wants to "look" towards the direction/ in the direction

	//Calculate angle between forward and direction

	//Angle calculation using determinant of the 2 vectors and (0,1,0)
	vCross = forward.Cross(direction);
	dot = forward.Dot(direction);
	det = Vector3::Up.Dot(vCross);
	rotationAngle = atan2(det, dot);

	//Create matrix for later
	Matrix m = Matrix::CreateRotationY(rotationAngle);
	//Matrix m = Matrix::CreateLookAt(forward, *GetPosition() + direction, Vector3::UnitZ);

	//rotate model by rotation
	pRotation->y += rotationAngle;

	//rotate forward vector to match
	Vector3::TransformNormal(forward, m, forward);

	forward.Normalize();



}

//Can be used to increase or decrease health
void Car::UpdateHealth(int amount)
{
	health += amount;
}

Car::~Car()
{

}
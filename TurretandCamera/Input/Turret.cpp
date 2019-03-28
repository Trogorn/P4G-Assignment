#include "Turret.h"



Turret::Turret()
{
}


Turret::~Turret()
{
}


void Turret::Shoot(int box)
{

	Ray ray;
	float shortestdistrance(distance);
	BoundingBox box;
	bool hit = false;


	if (boxColiders.capacity > 0)
		for (int i(0); i < boxColiders.capacity; i++)
		{
			float dummydistance(distance);
			if (ray.Intersects(boxColiders[i], _Out_ dummydistance) && dummydistance < shortestdistrance)
			{
				shortestdistrance = dummydistance;
				box = boxColiders[i];
				hit = true;
			}


		}

	if (hit)
	{
		hitPosition = origin + direction * shortestdistrance;

		//Damage linked reference in box if it has one
	}
	else
		hitPosition = Vector3(0, 0, 0);

}
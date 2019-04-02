#pragma once
#include "CollideInto.h"
class AmmoPickup :
	private CollideInto
{
public:
	AmmoPickup();
	~AmmoPickup();
	int BestowAmmo(/*CarAmmo*/);
private:
	int BestowedAmmunition = 50;
};


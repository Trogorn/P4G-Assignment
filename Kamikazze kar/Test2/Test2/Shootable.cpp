#include "Shootable.h"



Shootable::Shootable()
{

}


Shootable::~Shootable()
{

}

void Shootable::TakeDamage(const int& Damage)
{

	Health = Health - Damage;

}

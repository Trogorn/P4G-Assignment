#pragma once
#include "GameObject.h"
class Shootable :
	private GameObject
{
public:
	Shootable();
	~Shootable();

	void TakeDamage(const int& Damage);



private:
	int Health = 1;
	int Damage = 1;
};


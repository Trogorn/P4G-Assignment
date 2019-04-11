#pragma once
#include "Shootable.h"
class Repair :
	private Shootable
{
public:
	Repair();
	~Repair();
	int RepairCar(/*int CarHealth*/);
private:
	int repairAmount = 10;
};


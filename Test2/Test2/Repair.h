#pragma once
#include "Shootable.h"
class Repair :
	private Shootable
{
public:
	Repair();
	~Repair();

private:
	int repairAmount = 10;
};


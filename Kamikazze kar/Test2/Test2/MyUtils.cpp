#include "MyUtils.h"



MyUtils::MyUtils()
{
}


MyUtils::~MyUtils()
{
}


float MyUtils::Deg2Rad(float deg)
{
	return (deg * deg2rad);
}

float MyUtils::Rad2Deg(float rad)
{
	return (rad * rad2deg);
}

float MyUtils::deg2rad = (3.14159265359f / 180.f);

float MyUtils::rad2deg = (180.f / 3.14159265359f);
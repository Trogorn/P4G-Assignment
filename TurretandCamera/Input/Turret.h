#pragma once

#include <Windows.h>
#include <math.h>
#include <cassert>
#include <sstream>


#include "Input.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "WindowUtils.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


class Turret
{
public:
	Turret();
	~Turret();
	void Shoot(Vector3 position, Vector3 direction, float distance);

private:
	Vector3 hitPosition = Vector3(0,0,0);
	Vector3 origin; Vector3 direction;
	float distance;
};


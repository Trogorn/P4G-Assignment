// by Mark Featherstone (C) 2015 All Rights Reserved.
#include "Error.h"

// Application includes
#include "Input.h"
#include "ShipController.h"

// Defines
#define STEERING_CHANGE_RATE	5.0f



ShipController::ShipController(ShipControlMethod::Enum aMethod) :
	mControlMethod(aMethod), mSteering(0.0f), mFire(false), mLastFired(0)
{
}


void ShipController::Update(float aTime)
{
	switch (mControlMethod)
	{
	case ShipControlMethod::SCM_KEY:
		UpdateFromKey(aTime);
		break;
	default:
		MPOD_ASSERT_MSG(false, "Unknown ControlMethod value");
	}
	//when you let go the steering doesn't snap stop
	if (!GetMouseAndKeys()->IsPressed(VK_LEFT) && !GetMouseAndKeys()->IsPressed(VK_RIGHT) && mSteering!=0)
		mSteering*=0.9f;
}

bool ShipController::GetThrusting() const
{
	return GetMouseAndKeys()->IsPressed(VK_UP);
}

bool ShipController::GetReversing() const
{
	return GetMouseAndKeys()->IsPressed(VK_DOWN);
}

void ShipController::UpdateFromKey(float aTime)
{
	//turn left and right
	float lDelta = aTime * STEERING_CHANGE_RATE;
	if (GetMouseAndKeys()->IsPressed(VK_LEFT))
	{
		mSteering -= lDelta;
		if (mSteering < -1.0f)
			mSteering = -1.0f;
	}
	else if (GetMouseAndKeys()->IsPressed(VK_RIGHT))
	{
		mSteering += lDelta;
		if (mSteering > 1.0f)
			mSteering = 1.0f;
	}
	else
	{
		if (mSteering > 0.0f)
		{
			mSteering -= lDelta;
			if (mSteering < 0.0f)
				mSteering = 0.0f;
		}
		else if (mSteering < 0.0f)
		{
			mSteering += lDelta;
			if (mSteering > 0.0f)
				mSteering = 0.0f;
		}
	}

	mFire=false;
	mLastFired+=aTime;
	if (GetMouseAndKeys()->IsPressed(VK_Z))
	{
		//slow repeat of fire held
		if(mLastFired>0.75f)
		{
			mFire=true;
			mLastFired=0;
		}
	}
	else
	{
		//if you press and release you can fire fast
		if(mLastFired<0.65f)
			mLastFired=0.65f;
	}
}


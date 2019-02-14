// by Mark Featherstone (C) 2015 All Rights Reserved.

#ifndef SHIPCONTROLLER_H
#define SHIPCONTROLLER_H



namespace ShipControlMethod { typedef enum
{
	SCM_KEY, SCM_PAD
} Enum; }


// This class sits between the input managers and
// the ship class in order to centre all player input requirements
// into a single place
class ShipController
{
public:
	ShipController(ShipControlMethod::Enum aMethod);
	~ShipController() {}


	// Update inputs
	void Update(float dTime);
	//turning left and right is managed to control the response rate
	float GetSteering() const	{ return mSteering; }

	bool GetThrusting() const;
	bool GetReversing() const;
	bool GetFiring() const { return mFire; }

private:
	//---------------------------------------------------------------
	// Private methods
	//---------------------------------------------------------------

	// Update keyboard input
	void UpdateFromKey(float aTime);

	// Specify current control method in use
	ShipControlMethod::Enum mControlMethod;

	// Current steering input.  Ranges from -1.0f for maximum left
	// movement to 1.0f for maximum right movement
	float mSteering;

	//firing repeats at a slow rate if the key is held
	//if the player taps away at it he can fire faster
	float mLastFired;
	bool mFire;
};

#endif
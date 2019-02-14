// by Mark Featherstone (C) 2015 All Rights Reserved.
// Class representing the players character

#ifndef SHIP_H
#define SHIP_H

// Application includes
#include "GameObject.h"
#include "ShipController.h"
#include "SpriteAnim.h"
#include "Explosion.h"


/*
The ship is the most complicated game object
*/
class Ship : public GameObject
{
public:
	//we can get the numeric type for a ship without creating one
	static int CalcType() { return GameObject::GetTypeFromName("SHIP"); }
	Ship();
	virtual ~Ship() {};

	//if we are moving then update it
	//also handles firing, spawning and exploding - they need extracting
	void Update(float dTime) override;
	//as long we aren't spawning in, if we hit anything we die
	void OnCollide(const GameObject* apOther) override;
	//render the ship
	void Render() override;
	//explosion, the thrust - all additive so need to render last
	void PostRender() override;
	//game object manager needs a way to make multiple objects, but we
	//only need one ship
	GameObject *Create() override { MPOD_ASSERT(false); return NULL; }
	//the ship is reused when you die, no new/delete, so it needs a way to 
	//initialise itself when it restarts
	void Init() override;
	//simple force handling, just add it onto our velocity, we don't take damage
	void AddForce(const DirectX::SimpleMath::Vector2 &force, float damage) override { 
		SetVelocity(GetVelocity() + force); 
	}

	//----------------------------------------------------------------
	// Public methods
	//----------------------------------------------------------------
	//need a new ship so reuse this one
	void RespawnNow() {
		mWaitingForRespawn=true;
		SetCanCollide(false);
		mRespawnTimer=-1;
	}

private:
	static const float MAX_TURN_SPEED;

	//rotate at a fixed rate 
	void UpdateAngle(float dTime);

	// Update position and speed 
	void UpdatePosition(float dTime);

	//protect us from the input implementation
	ShipController	mController;
	//go bang when we die
	Explosion mExplosion;
	//additive animated texture trail
	unsigned int mThrustHdl;	//thrust sound effect instance we can manipulate
	
	//need to be careful when respawning so we don't just die straight
	//away, these members let us control the respawn
	float mRespawnTimer;
	bool mWaitingForRespawn;
	SpriteAnim mThrust;
	unsigned int mLightHdl = UDEF::MAX_UINT;
};

#endif

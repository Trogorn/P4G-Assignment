// by Mark Featherstone (C) 2015 All Rights Reserved.
#ifndef ASTEROID_H
#define ASTEROID_H

#include "d3d.h"

// Application includes
#include "GameObject.h"
#include "Explosion.h"

// Forward declarations


//just an enum that won't pollute any other class
enum class AsteroidType {
	SMALL,
	MEDIUM,
	LARGE
}; 

/*
Asteroids float around, bang off the edge of the world, the player and each other.
As they blow up they create smaller asteroids (large->med->small), small ones
just blow up.
*/
class Asteroid : public GameObject
{
public:
	//given a unique text name for this type, create a unique numeric ID
	static int CalcType() { return GameObject::GetTypeFromName("ASTEROID"); }
	Asteroid();
	virtual ~Asteroid();	//has to be virtual as we are inheriting from an abstract type

	/*
	Provide concrete code for the abstract bits
	*/
	void Update(float dTime) override;
	void OnCollide(const GameObject* apOther) override;
	void Render() override;
	void PostRender() override;
	GameObject *Create() override { return new Asteroid; }
	void Init() override;
	void AddForce(const DirectX::SimpleMath::Vector2 &force, float damage) override;

	//----------------------------------------------------------------
	// Public methods
	//----------------------------------------------------------------
	//a homing asteroid chases the player
	void SetHoming(bool state) { mHoming=state; }
	//an asteroid has it's own type small/med/large
	void SetType(AsteroidType type);
	AsteroidType GetAsteroidType() const { return mAsteroidType; }

private:

	DirectX::SimpleMath::Vector2 mDir;			//our direction
	DirectX::SimpleMath::Vector2 mRot, mRotInc;	//asteroids rotate randomly in 2 axes
	float mHealth;			//asteroids take so much damage and then blow up
	AsteroidType mAsteroidType;	//small-med-large
	float mTimeSecs;		//a clock that counts up from when we were spawned
	Explosion mExplosion;	//we need an explosion effect when we blow up
	bool mHoming;			//chase the player if true
	unsigned int mSfxHdl;	//make a noise if we nudge something, so need to track the sfx as it plays

	//if two asteroids hit they damage each other, but it depends on the size of each one
	float GetDamage(const Asteroid *pOther) const;	
	//if something does us damage we need to respond - play a sfx 
	void TakeDamage(float damage, const GameObject *pOther=NULL);
};





#endif
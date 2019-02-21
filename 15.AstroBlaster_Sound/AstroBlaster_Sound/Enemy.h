// by Mark Featherstone (C) 2013 All Rights Reserved.
#ifndef ENEMY_H
#define ENEMY_H

#include "SimpleMath.h"
// Application includes
#include "GameObject.h"
#include "Explosion.h"



/*
A copy of the Asteroid class with some minor tweaks
*/
class Enemy : public GameObject
{
public:
	//given a unique text name for this type, create a unique numeric ID
	static int CalcType() { return GameObject::GetTypeFromName("ENEMY"); }
	Enemy();
	virtual ~Enemy();	//has to be virtual as we are inheriting from an abstract type

	/*
	Provide concrete code for the abstract bits
	*/
	void Update(float aTime) override;
	void OnCollide(const GameObject* apOther) override;
	void Render() override;
	void PostRender() override;
	GameObject *Create() override { return new Enemy; }
	void Init() override;
	void AddForce(const DirectX::SimpleMath::Vector2 &force, float damage) override;

	//----------------------------------------------------------------
	// Public methods
	//----------------------------------------------------------------
	//sometimes we want to override normal rendering and put the asteroid somewhere
	//void Render(D3DXMATRIX &WorldMat);

private:

	DirectX::SimpleMath::Vector2 mDir;			//our direction
	float mRot, mRotInc;	//enemy barrels rotate randomly
	float mHealth;			//barrels take so much damage and then blow up
	float mLifeSecs;		//a clock that counts up from when we were spawned
	Explosion mExplosion;	//we need an explosion effect when we blow up
	unsigned int mSfxHdl;	//make a noise if we nudge something, so need to track the sfx as it plays

	//if something does us damage we need to respond - play a sfx
	void TakeDamage(float damage, const GameObject *pOther=NULL);
};





#endif
// by Mark Featherstone (C) 2015 All Rights Reserved.
#ifndef BULLET_H
#define BULLET_H


// Application includes
#include "GameObject.h"
#include "SpriteAnim.h"

class Sprite;

/*
the player fire off bullets, the go in a straight line and die quickly
*/
class Bullet : public GameObject
{
public:
	static int CalcType() { return GameObject::GetTypeFromName("BULLET"); }
	Bullet();
	virtual ~Bullet();	//we've got an abstract base so need a virtual destructor

	// Virtual methods from GameObject class
	void Update(float dTime) override;
	void OnCollide(const GameObject* apOther) override;
	void Render() override {}
	void PostRender() override;
	GameObject *Create() override { return new Bullet; }
	bool GetResultantMomentum(DirectX::SimpleMath::Vector2 &vel, GameObject *pCollider) override { return false; }
	void Init() override;
	void AddForce(const DirectX::SimpleMath::Vector2 &force, float damage) override {}

	//we don't want bullets blowing up the ship as the leave, so
	//get the type of the ship so we can ignore it
	void SetSpawnerType(unsigned int type) { mSpawnerType=type; }


private:

	void Release();

	//Sprite *mpSprite;			//bullets appear as a small sprite
	float mLife;				//bullets don't live very long
	unsigned int mSpawnerType;  //remember where we came from
	unsigned int mLightHdl;		//bullets carry a point light with them
};




#endif
// by Mark Featherstone (C) 2015 All Rights Reserved.
#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <string>
#include "SpriteAnim.h"
#include "d3d.h"

/*
We like things to go bang a lot so it makes sense ot have
a reusable class. It plays an animated texture explosion
and creates a force that damages and blows things away.
Also lights up the scene.
*/
class Explosion
{
public:
	Explosion();
	~Explosion();
	//we reuse things, no run time allocation
	void Init();
	//keep the anim playing
	void Update(float dTime);
	//transparent additive effect so needs rendering after everything else
	void PostRender();
	//is it still exploding?
	bool GetIsPlaying() const;
	//how many times has the explosion played completely
	int GetNumPlays() const;
	//start the animation playing at a specific position, lit or unlit, a sfx to play
	void Play(const DirectX::SimpleMath::Vector2 &pos, bool lit, const std::string &sfx);
	//set how big th explosion is (radius)
	void SetSize(float sz);
	float GetSize() const{
		return mSprite.GetScale().x;
	}

private:
	SpriteAnim mSprite;		//the texture animation
	unsigned int mLightHdl;	//the light we'll use
	float mTimer;			//used to help us expand and then contract the explosion's light
	DirectX::SimpleMath::Vector2 mPos;
	float mScale;
};

#endif
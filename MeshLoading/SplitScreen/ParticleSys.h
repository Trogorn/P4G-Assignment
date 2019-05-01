#pragma once

#include <d3d11.h>
#include <vector>
#include <algorithm>

#include "SimpleMath.h"
#include "Mesh.h"
#include "FX.h"

/*todo
keys to animate particle properties over their lifetime
gravity is just a 2nd accelerating force and should be configurable, not always pointing down
add colour to the vertex so the particle has control of alpha, that way you can fade particles out smoothly
*/

/*
There are potentially tens of thousands of instances describing
a single particle moving through the world
*/
class Particle
{
public:
	//world space
	DirectX::SimpleMath::Vector3 mPos;
	//where is it going and how fast
	DirectX::SimpleMath::Vector3 mVel;
	//current rotation of the sprite in radians
	float mAngle = 0;
	//how fast and which way is it rotating
	float mRotSpeed = 0;
	//track how long it should live, once dead it's freed to be used again
	float mLifeSec = 0, mSpawnSec = 0;
	//how big is the sprite
	float scale = 1;
	//which sprite it should use
	int mSpriteIdx = 0;
	//particles are organised in a linked list so it's easy to find all the ones that need
	//rendering (busy) and all the ones that are free to use (free)
	Particle *mpPrev = nullptr, *mpNext = nullptr;

	//bit flags
	enum {
		USE_GRAVITY = 1,
		ALIVE = 2		//marked alive when it becomes active and starts rendering
	};
	unsigned int mFlags = 0;

	//gravitational force
	static const float GRAVITY;
};

/*
There are 20~30 emitters, each instance generates particles over time and sets the
particles initial parameters
*/
class ParticleSys;
class Emitter
{
public:

	//configuring an emitter
	enum class Type { 
		OMNI,		//emit in all directions
		DIRECTION	//emit in a specific direction
	};
	struct Data
	{
		//where is the emitter in the world
		DirectX::SimpleMath::Vector3 mPos;
		//time between emissions
		float mEmitDelaySec = 0;
		//how many to emit at once
		int mNumEmit = 0;
		//how many to emit in total, -1 means keep going forever
		int mTotal = 0;
		//how should the emitter emit
		Type mType = Type::OMNI;
		//a random size between these two limits
		DirectX::SimpleMath::Vector2 mScaleMinMax = DirectX::SimpleMath::Vector2(1, 1);
		//initial sprite rotation (radians)
		DirectX::SimpleMath::Vector2 mInitRotMinMax = DirectX::SimpleMath::Vector2(0, 0);
		//how fast and which direction should sprites spin (radians per sec)
		DirectX::SimpleMath::Vector2 mRotSpMinMax = DirectX::SimpleMath::Vector2(0, 0);
		//any initial flags on the particle (ALIVE is always one of them)
		int mParticleFlags = 0;
		//how long should a particle live
		DirectX::SimpleMath::Vector2 mInitLifeSecMinMax = DirectX::SimpleMath::Vector2(1, 1);
		//should this emitter return itself to the free pool once it runs out of particles?
		bool mReleaseOnEmpty = false;
		//emitter orientation - important if type=DIRECTION
		DirectX::SimpleMath::Matrix mOrientation;
		//if emitting in a specific direction, this lets you randomly vary that direction (in radians)
		DirectX::SimpleMath::Vector2 mInitAngleMinMax = DirectX::SimpleMath::Vector2(0, 0);
		//which sprite to use
		int mSpriteIdx = 0;
		//how fast are sprites moving
		DirectX::SimpleMath::Vector2 mInitSpeedMinMax = DirectX::SimpleMath::Vector2(1, 1);
	};
	Data mData;
	//track how often we've been emitting
	float mEmitDelaySec = 0, mLastEmitSec = 0;
	//if true then this emitter is emitting
	bool mAlive = false;

	//keep it going
	void Update(ParticleSys& ps);
	
	//guess
	void StopAndRelease() {
		mAlive = false;
	}

private:

	//an emitter wanting to emit some particles
	bool CreateParticle(ParticleSys& ps);
};

/*
Manages the emitters and particles, as a manager this is a good candidate for the singleton pattern really
*/
class ParticleSys
{
public:
	//how many particles are active at any one time
	int mNumParticles = 0;
	//upper limits on the system
	const int MAX_PARTICLES = 40000;
	const int MAX_EMITTERS = 20;

	//start the whole thing up
	void InitialisePS();
	//keep it going
	//dTime=elapsed time per frame
	//camPos=the camera world position (sprites face the camera always)
	void UpdateParticles(float dTime, const DirectX::SimpleMath::Vector3& camPos);
	//there are only 20 emitters, so it's possible to ask for one and get nullptr
	Emitter* GetNewEmitter() {
		for(auto& e : mEmitters)
			if (!e.mAlive)
			{
				e.mAlive = true;
				return &e;
			}
		return nullptr;
	}
	//render particles last, the z buffer is usually disabled (it doesn't have to be)
	void Render(FX::MyFX& fx);
	//shut it all down and release everything
	void ReleasePS();

	//get hold of the internal particle buffers
	Particle *&GetFreeList() {
		return mpFreeList;
	}
	Particle *&GetBusyList() {
		return mpBusyList;
	}

	//texture coordinates of rectangles in the sprite atlas
	struct TexRect
	{
		float u, v, w, h;  //these are resolution independant sprite texture coordinates (so 0->1=full texture)
	};
	std::vector<TexRect>& GetSpriteLibrary() { return mSprites; }

private:
	//linked list particle buffers, it's done this way to avoid searching
	Particle *mpFreeList = nullptr, *mpBusyList = nullptr;
	//all the emitters POD
	std::vector<Emitter> mEmitters;
	//all the particles POD
	std::vector<Particle> mParticles;
	//dynamic vbuffer to render sprites into, index buffer can be static
	ID3D11Buffer *pVBuffer;
	ID3D11Buffer *pIBuffer;
	//the sprite library
	std::vector<TexRect> mSprites;
};
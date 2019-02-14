// by Mark Featherstone (C) 2015 All Rights Reserved.


// Application includes
#include "CRC.h"
#include "Camera.h"
#include "GameObject.h"
#include "D3D.h"
#include "fx.h"

//instead of giving game objects a text name, use the text
//to generate a unique ID
int GameObject::GetTypeFromName(const char *pType)
{
	return SCRC::GenerateCRC32(pType, strlen(pType));
}


//game objects need a type
GameObject::GameObject(const char* apType) :
	mAngle(0.0f), mRadius(0.0f), mInUse(false), 
		mCanCollide(true), mVelocity(0,0), mPosition(0,0), mVelLimit(10)
{
	mType = GetTypeFromName(apType);
}


// Update state of this game object
//we can only do the bit common to all game objects
void GameObject::Update(float aTime)
{
	// Calculate how far we've moved this frame
	float l2=mVelocity.LengthSquared();
	if(l2>(mVelLimit*mVelLimit))
	{
		mVelocity/=sqrtf(l2);
		mVelocity*=mVelLimit;
	}

	// Update position
	SetPosition( GetPosition() + mVelocity * aTime);

}

//we assume all game objects can render themselves
void GameObject::Render()
{
	FX::MyFX& fx = *FX::GetMyFX();
	fx.Render(mModel, gd3dImmediateContext);
}



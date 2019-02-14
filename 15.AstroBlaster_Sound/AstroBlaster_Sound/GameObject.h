// by Mark Featherstone (C) 2015 All Rights Reserved.
// Base class representing an in game object

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "d3d.h"

#include "GameObjectManager.h"
#include "Model.h"

// Forward declarations


/*
All game objects share some common behaviour
*/
class GameObject
{
/*
no other code outside a game object can delete one, to stop any accidents.
However, the game object manager is the only thing that IS allowed, so we make
its destructor a friend, then that function can access the private/protected 
parts of any game object to delete it when the game exits
*/
friend GameObjectManager::~GameObjectManager(); 
public:
	static int GetTypeFromName(const char *pType);
	GameObject(const char* apType);

	// Updates the current position etc. of the game object.  Can be
	// overridden by derived class to provide additional functionality
	virtual void Update(float dTime);

	// Render the game object on screen at its current position using
	// its current model.  Can be overridden to provide additional
	// functionality if required
	virtual void Render();
	//sometimes we want to render after everything else, additive sprites for instance
	virtual void PostRender() {};

	// Called when this object collides with another object
	virtual void OnCollide(const GameObject* apOther)	{}

	/*
	We need a way to be able to dynamically create game objects, but that means knowing
	the concrete type of each one (not the abstract type), so how can we do it abstractly?
	If we have one instance, we can call this function to create another, a copy of it. The 
	caller doesn't need to know anything about the concrete type, he just needs one instance.
	*/
	virtual GameObject *Create() =0; //=0 means a pure abstract function, so children must provide concrete versions

	//if we hit something we want to take their momentum when we bounce away
	virtual bool GetResultantMomentum(DirectX::SimpleMath::Vector2 &vel, GameObject *pCollider) { vel=mVelocity; return true; }

	//objects are not dynamically allocated at run time so we need a way to reset an object
	virtual void Init() {};

	//when things blow up the force blows other object about and does damage to them
	virtual void AddForce(const DirectX::SimpleMath::Vector2 &force, float damage) = 0;

	// Get the type of this game object
	unsigned int GetType() const						{ return mType; }

	// Get access to objects position
	const DirectX::SimpleMath::Vector2&	GetPosition() const			{ return mPosition; }

	// Update objects position
	void SetPosition(const DirectX::SimpleMath::Vector2& arPos)
	{ 
		MPOD_ASSERT_MSG(arPos.x>-9999 && arPos.x<9999 && arPos.y>-9999 && arPos.y<9999,"Bad position");
		mPosition = arPos;
		mModel.GetPosition() = DirectX::SimpleMath::Vector3(arPos.x, arPos.y, 0);
	}

	// Get access to objects velocity
	const DirectX::SimpleMath::Vector2&	GetVelocity() const			{ return mVelocity; }

	// Update objects velocity
	void SetVelocity(const DirectX::SimpleMath::Vector2& arVel)
	{ 
		MPOD_ASSERT_MSG(arVel.x>-9999 && arVel.x<9999 && arVel.y>-9999 && arVel.y<9999,"Bad velocity");
		mVelocity = arVel; 
	}
	//we don't want anything going crazy fast, this is a terminal velocity hard limit on speed
	void SetVelocityLimit(float limit)
	{
		MPOD_ASSERT_MSG(limit>=0 && limit<9999,"Bad velocity limit");
		mVelLimit=limit;
	}

	// Set the model used to render this object
	void SetModel(Model& model) { 
		mModel = model; 
	}
	Model &GetModel() { 
		return mModel; 
	}

	// Get current Y rotation
	float GetRotY() const						{ return mAngle; }

	// Set current Y rotation
	void SetRotY(float aAngle)					{ mAngle = aAngle; }

	// Set the collision radius
	void SetRadius(float aRadius)				{ mRadius = aRadius; }

	// Get the collision radius
	float GetRadius() const						{ return mRadius; }

	//is this object sat in the cache of free ones or is it actually in use
	bool GetInUse() const { return mInUse; }
	void SetInUse(bool inUse) { mInUse=inUse; }

	//should it be involved in collision tests
	bool GetCanCollide() const { return mCanCollide; }
	void SetCanCollide(bool canCollide) { mCanCollide=canCollide; }

protected:
	/*
	no code can delete a game object other than the cache maanager where they live
	abstract destructors like this have to be virtual or things won't get destroyed properly
	*/
	virtual ~GameObject() {};  

private:
	DirectX::SimpleMath::Vector2	mPosition;

	// Current velocity of this game object
	DirectX::SimpleMath::Vector2	mVelocity;
	float mVelLimit;	//hard limit

	// its a 2d world so what direction are we facing?
	float	mAngle;

	// Model to use when rendering this game object
	Model mModel;

	// Identifier value created by hashing of the string value passed into
	// the GameObject constructor
	unsigned int mType;

	// Collision radius
	float mRadius;
	// is it being used in game?
	bool mInUse;
	//can it hit things 
	bool mCanCollide;
};

#endif
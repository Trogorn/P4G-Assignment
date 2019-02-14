// by Mark Featherstone (C) 2015 All Rights Reserved.

#ifndef CAMERA_H
#define CAMERA_H

#include "d3d.h"

#include "UtilsDefines.h"

// Forward declarations
class GameObject;

//-------------------------------------------------------------------
// Camera class
//-------------------------------------------------------------------
class Camera
{
public:
	Camera() : mpTarget(0), mPosition(0,0), mZ(-10), mTargetLag(1), 
		mPositionLag(1), mMin(-UDEF::MAX_REAL,-UDEF::MAX_REAL), mMax(UDEF::MAX_REAL,UDEF::MAX_REAL) {}

	// Get the current camera position
	const DirectX::SimpleMath::Vector2& GetPosition() const	{ return mPosition; }

	// Update the camera details
	void Update(float dTime);
	//update the matrices
	void PreRender();
	// Set the game object that we're looking at
	void SetTarget(GameObject* apTarget)	{ mpTarget = apTarget; }
	//force the camera instantly into a new position
	void SetPosition(const DirectX::SimpleMath::Vector2 &camPos) {
		mpTarget=0;
		mPosition=camPos;
		mTarget=camPos;
	}
	//2d scene but the 
	void SetCamZ(float dist) { mZ=dist; }
	void SettargetLag(float lag) { mTargetLag=lag; }
	void SetPositionLag(float lag) { mPositionLag=lag; }
	void SetMoveLimits(const DirectX::SimpleMath::Vector2 &min, const DirectX::SimpleMath::Vector2 &max) { mMin = min; mMax = max; }
	bool SphereInFrustum(const DirectX::SimpleMath::Vector3 &position, float radius);
	float GetNearZ() const { return mZBuffDim.x; }
	float GetFarZ() const { return mZBuffDim.y; }

private:
	// The position of the camera in the world
	DirectX::SimpleMath::Vector2 mPosition, mTarget;
	DirectX::SimpleMath::Vector2 mMin, mMax;
	// The game object the camera is tracking
	GameObject* mpTarget;					//camera target follows this point
	float mZ;				//its a 2d view but we still need to fix the camera Z to set how much is visible
	float mTargetLag;		//keep the camera's movements elastic
	float mPositionLag;		
	DirectX::SimpleMath::Plane mfrustum[6];	//6 planes to define what the camera can see
	DirectX::SimpleMath::Vector2 mZBuffDim{ 1, 5000 };

	//when we change the camera matrices we rebuild the frustum
	void BuildViewFrustum();
	void SetupProjectionMatrix();
	void SetupViewMatrix();
};

void Local3DtoScreenSpace(DirectX::SimpleMath::Vector2& spos, const DirectX::SimpleMath::Vector3& localPos, DirectX::SimpleMath::Matrix& worldTfm, float nearZ, float farZ);

#endif
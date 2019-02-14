// by Mark Featherstone (C) 2015 All Rights Reserved.

#include "Camera.h"
#include "GameObject.h"
#include "Error.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "WindowUtils.h"
#include "FX.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/*
XMVector3Project seems to crash in the lab, maybe VS2017 isn't configured properly?
We can just do it by hand based on the old D3DX code though
D3DXVECTOR3* WINAPI D3DXVec3Project(D3DXVECTOR3 *pout, CONST D3DXVECTOR3 *pv, CONST D3DVIEWPORT8 *pviewport, CONST D3DXMATRIX *pprojection, CONST D3DXMATRIX *pview, CONST D3DXMATRIX *pworld)
{
D3DXMATRIX m1, m2;
D3DXVECTOR3 vec;

D3DXMatrixMultiply(&m1, pworld, pview);
D3DXMatrixMultiply(&m2, &m1, pprojection);
D3DXVec3TransformCoord(&vec, pv, &m2);
pout->x = pviewport->X +  ( 1.0f + vec.x ) * pviewport->Width / 2.0f;
pout->y = pviewport->Y +  ( 1.0f - vec.y ) * pviewport->Height / 2.0f;
pout->z = pviewport->MinZ + vec.z * ( pviewport->MaxZ - pviewport->MinZ );
return pout;
}*/

void Local3DtoScreenSpace(Vector2& spos, const Vector3& localPos, Matrix& worldTfm, float nearZ, float farZ)
{
	int w, h;
	GetClientExtents(w, h);
	//local space to object space
	Vector3 mp = Vector3::TransformNormal(localPos, worldTfm);
	// XMVector4Transform(localPos, worldTfm);
	//object space to screen space
	//Vector3 pt = XMVector3Project(mp, 0, 0, (float)w, (float)h, nearZ, farZ, FX::GetProjectionMatrix(), FX::GetViewMatrix(), worldTfm);

	Matrix m1 = worldTfm * FX::GetViewMatrix() * FX::GetProjectionMatrix();
	Vector3 tvec = Vector3::Transform(mp, m1);
	Vector3 pt;
	pt.x = 0 + (1 + tvec.x) * w / 2;
	pt.y = 0 + (1 - tvec.y) * h / 2;
	pt.z = nearZ + tvec.z * (farZ - nearZ);


	spos.x = pt.x;
	spos.y = pt.y;
}


//stop a vector moving outside fixed limits
void Clamp(Vector2 &pos, const Vector2 &min, const Vector2 &max)
{
	if(pos.x<min.x) 
		pos.x=min.x;
	else if(pos.x>max.x) 
		pos.x=max.x;
	if(pos.y<min.y) 
		pos.y=min.y;
	else if(pos.y>max.y) 
		pos.y=max.y;
}

void Camera::Update(float aTime)
{
	if(mpTarget)
	{
		//make the camera look at point follow the target up to a certain point
		mTarget += Vector2(mpTarget->GetPosition().x - mTarget.x, mpTarget->GetPosition().y - mTarget.y) * mTargetLag*aTime;
		Clamp(mTarget,mMin,mMax);
	}
	//make the camera position follow a target position to a certain point
	mPosition += Vector2(mTarget.x-mPosition.x, mTarget.y-mPosition.y) * mPositionLag * aTime;
	Clamp(mPosition,mMin,mMax);

	SetupViewMatrix(); 
}

void Camera::PreRender()
{
	FX::SetPerFrameConsts(gd3dImmediateContext, Vector3(mPosition.x, mPosition.y, mZ));
	SetupViewMatrix();
	SetupProjectionMatrix();
}

void Camera::SetupViewMatrix()
{
	// Set up the view matrix.
	// This defines which way the 'camera' will look at, and which way is up.
	Vector3 vCamera(mPosition.x, mPosition.y, mZ);
	Vector3 vLookat(mTarget.x, mTarget.y, 0.0f);
	Vector3 vUpVector(0.0f, 1.0f, 0.0f);
	CreateViewMatrix(FX::GetViewMatrix(), vCamera, vLookat, vUpVector);
	BuildViewFrustum();
}

void Camera::SetupProjectionMatrix()
{
	// Set up the projection matrix.
	// This transforms 2D geometry into a 3D space.
	int w, h;
	GetClientExtents(w, h);
	CreateProjectionMatrix(FX::GetProjectionMatrix(), PI / 4, (float)w / (float)h, mZBuffDim.x, mZBuffDim.y);
}

/*
Everything the camera can actually see is defined by the frustum
looks a bit like a pyramid. If we actually define it as six planes
we can work out whether an object is visible or not
*/
void Camera::BuildViewFrustum()
{
	Matrix viewProjection;
	viewProjection = FX::GetViewMatrix() * FX::GetProjectionMatrix();
	// Left plane
	mfrustum[0].x = viewProjection._14 + viewProjection._11;
	mfrustum[0].y = viewProjection._24 + viewProjection._21;
	mfrustum[0].z = viewProjection._34 + viewProjection._31;
	mfrustum[0].w = viewProjection._44 + viewProjection._41;

	// Right plane
	mfrustum[1].x = viewProjection._14 - viewProjection._11;
	mfrustum[1].y = viewProjection._24 - viewProjection._21;
	mfrustum[1].z = viewProjection._34 - viewProjection._31;
	mfrustum[1].w = viewProjection._44 - viewProjection._41;

	// Top plane
	mfrustum[2].x = viewProjection._14 - viewProjection._12;
	mfrustum[2].y = viewProjection._24 - viewProjection._22;
	mfrustum[2].z = viewProjection._34 - viewProjection._32;
	mfrustum[2].w = viewProjection._44 - viewProjection._42;

	// Bottom plane
	mfrustum[3].x = viewProjection._14 + viewProjection._12;
	mfrustum[3].y = viewProjection._24 + viewProjection._22;
	mfrustum[3].z = viewProjection._34 + viewProjection._32;
	mfrustum[3].w = viewProjection._44 + viewProjection._42;

	// Near plane
	mfrustum[4].x = viewProjection._13;
	mfrustum[4].y = viewProjection._23;
	mfrustum[4].z = viewProjection._33;
	mfrustum[4].w = viewProjection._43;

	// Far plane
	mfrustum[5].x = viewProjection._14 - viewProjection._13;
	mfrustum[5].y = viewProjection._24 - viewProjection._23;
	mfrustum[5].z = viewProjection._34 - viewProjection._33;
	mfrustum[5].w = viewProjection._44 - viewProjection._43;

	// Normalize planes
	for (int i = 0; i < 6; i++)
		mfrustum[i].Normalize(); //plane normalize
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Checks whether a sphere is inside the camera's view frustum.
Parameters:
[in] pPosition - Position of the sphere.
[in] radius - Radius of the sphere.
Returns: TRUE if the sphere is in the frustum, FALSE otherwise
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool Camera::SphereInFrustum(const Vector3 &position, float radius )
{
	for ( int i = 0; i < 6; i++ )
	{
		if (mfrustum[i].DotCoordinate(position) + radius < 0)
		{
			// Outside the frustum, reject it!
			return FALSE;
		}
	}
	return TRUE;
}

#ifndef MODEL_H
#define MODEL_H

#include <cassert>
#include "d3d.h"
#include "ShaderTypes.h"
#include "Mesh.h"

class Mesh;

 


class Model
{
public:
	void Initialise(Mesh &mesh, bool terrain = 0);
	
	DirectX::SimpleMath::Vector3& GetPosition() { return mPosition; }
	DirectX::SimpleMath::Vector3& GetScale() { return mScale; }
	DirectX::SimpleMath::Vector3& GetRotation() { return mRotation; }
	DirectX::SimpleMath::Vector3& GetPreRotateOffset() { return mOffset; }
	void GetWorldMatrix(DirectX::SimpleMath::Matrix& w);
	Mesh& GetMesh() {
		assert(mpMesh);
		return *mpMesh; 
	}
	MaterialExt* HasOverrideMat() {
		if (mUseOverrideMat)
			return &mOverrideMaterial; 
		return nullptr;
	}
	void SetOverrideMat(MaterialExt *pMat = nullptr) {
		if (!pMat) {
			mUseOverrideMat = false;
			return;
		}
		mUseOverrideMat = true;
		mOverrideMaterial = *pMat;
	}
	Model& operator=(const Model& m)
	{
		mpMesh = m.mpMesh;
		mPosition = m.mPosition;
		mScale = m.mScale;
		mRotation = m.mRotation;
		mOverrideMaterial = m.mOverrideMaterial;
		mUseOverrideMat = m.mUseOverrideMat;
		return *this;
	}






	DirectX::BoundingOrientedBox& GetCollider()
	{
		return collider;
	}
	void ColiderUpdate()
	{
		collider = DirectX::BoundingOrientedBox(mPosition, DirectX::XMFLOAT3(mScale.x, mScale.y, mScale.z), DirectX::XMFLOAT4(0, 0, mRotation.z, 1.f));
	}

	void Die() { if(!terrain)alive = false; }
	bool GetAlive() { return alive; }
	bool GetTerrain() { return terrain; };





private:
	bool terrain;
	bool alive = true;
	DirectX::BoundingOrientedBox collider;
	Mesh *mpMesh = nullptr;
	DirectX::SimpleMath::Vector3 mPosition{ 0, 0, 0 }, mScale{ 1, 1, 1 }, mRotation{ 0, 0, 0 };
	DirectX::SimpleMath::Vector3 mOffset{ 0,0,0 }; //used if the artists supplied models won't line up 
	MaterialExt mOverrideMaterial;
	bool mUseOverrideMat = false;
};

#endif


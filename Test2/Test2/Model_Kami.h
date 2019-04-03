#ifndef MODEL_KAMI_H
#define MODEL_KAMI_H

#include <cassert>
#include "SimpleMath.h"
#include "ShaderTypes.h"

class Mesh;

//I will look at potentially getting this renamed back and seeing what effect this has on stuff (Lochlann)
class Model_Kami
{
public:

	//void Initialise(const std::string& meshFileName);
	void Initialise(Mesh &mesh);
	
	DirectX::SimpleMath::Vector3* GetPosition() { return &mPosition; }
	DirectX::SimpleMath::Vector3* GetScale() { return &mScale; }
	DirectX::SimpleMath::Vector3* GetRotation() { return& mRotation; }
	void GetWorldMatrix(DirectX::SimpleMath::Matrix& w);
	
	DirectX::BoundingOrientedBox& GetColider()
	{
		return colider;
	}
	void ColiderUpdate()
	{
		colider = DirectX::BoundingOrientedBox(mPosition, DirectX::XMFLOAT3(mScale.x, mScale.y, mScale.z), DirectX::XMFLOAT4(0, 0, mRotation.z,1.f));
	}

	void Die() { alive = false; }

	Mesh& GetMesh() 
	{
		assert(mpMesh);
		return *mpMesh;
	}

	bool RetAlive() { return alive; }

	MaterialExt* HasOverrideMat()
	{
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

	Model_Kami& operator=(const Model_Kami& m)
	{
		mpMesh = m.mpMesh;
		mPosition = m.mPosition;
		mScale = m.mScale;
		mRotation = m.mRotation;
		mOverrideMaterial = m.mOverrideMaterial;
		mUseOverrideMat = m.mUseOverrideMat;
		return *this;
	}

private:

	bool alive = true;
	Mesh *mpMesh = nullptr;
	DirectX::SimpleMath::Vector3 mPosition, mScale, mRotation;
	MaterialExt mOverrideMaterial;
	bool mUseOverrideMat = false;
	DirectX::BoundingOrientedBox colider;
};

#endif


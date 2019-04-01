#ifndef MODEL_KAMI_H
#define MODEL_KAMI_H

#include <cassert>
#include "SimpleMath.h"
#include "ShaderTypes.h"

class Mesh;

class Model_Kami
{
public:

	//void Initialise(const std::string& meshFileName);
	void Initialise(Mesh &mesh);
	
	DirectX::SimpleMath::Vector3& GetPosition() { return mPosition; }
	DirectX::SimpleMath::Vector3& GetScale() { return mScale; }
	DirectX::SimpleMath::Vector3& GetRotation() { return mRotation; }
	void GetWorldMatrix(DirectX::SimpleMath::Matrix& w);

	Mesh& GetMesh() 
	{
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

	Mesh *mpMesh = nullptr;
	DirectX::SimpleMath::Vector3 mPosition, mScale, mRotation;
	MaterialExt mOverrideMaterial;
	bool mUseOverrideMat = false;
};

#endif


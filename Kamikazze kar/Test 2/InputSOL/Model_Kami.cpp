#include "FX.h"
#include "Model_Kami.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Model_Kami::Initialise(Mesh &mesh)
{
	mpMesh = &mesh;			     
	mPosition = Vector3(0, 0, 0);
	mScale = Vector3(1, 1, 1);   
	mRotation = Vector3(0, 0, 0); 
	ColiderUpdate();
}

void Model_Kami::GetWorldMatrix(DirectX::SimpleMath::Matrix& w)
{
	w = Matrix::CreateScale(mScale) * Matrix::CreateRotationX(mRotation.x) *
		Matrix::CreateRotationY(mRotation.y) * Matrix::CreateRotationZ(mRotation.z) *
		Matrix::CreateTranslation(mPosition);
}

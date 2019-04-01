#include "FX.h"
#include "Model_Kami.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/*Vector3 AvoidQuad(float y, float minX, float maxX, float minZ, float maxZ, float minXAvoid, float maxXAvoid, float minZAvoid, float maxZAvoid) {
/*Parameters explained
float y: the actual Y value at which you want to spawn
float minX, maxX: the minimum and maximum value for X in the TOTAL map quad
float minZ, maxZ:  the minimum and maximum value for Z in the TOTAL map quad
float minXAvoid, maxXAvoid: the minimum and maximum value for X in the AVOIDED quad
float minZAvoid, maxZAvoid:  the minimum and maximum value for Z in the AVOIDED quad

while loop keeps generating X and Z values until the point is not within the avoided quad

The vector 3 returned has float y parameter and an X and Z value outside the avoided quad
*/
/*
float returnX = minXAvoid;
float returnZ = minZAvoid;
while ((returnX <= maxXAvoid && returnX >= minXAvoid) && (returnZ <= maxZAvoid && returnZ >= minZAvoid)) {
returnX = GetRandom(minX, maxX);
returnZ = GetRandom(minZ, maxZ);
}
return Vector3(returnX, y, returnZ);
}*/

void Model_Kami::Initialise(Mesh &mesh)
{
	mpMesh = &mesh;
	mPosition = Vector3(0, 0, 0);
	mScale = Vector3(1, 1, 1);
	mRotation = Vector3(0, 0, 0);
}

void Model_Kami::GetWorldMatrix(DirectX::SimpleMath::Matrix& w)
{
	w = Matrix::CreateScale(mScale) * Matrix::CreateRotationX(mRotation.x) *
		Matrix::CreateRotationY(mRotation.y) * Matrix::CreateRotationZ(mRotation.z) *
		Matrix::CreateTranslation(mPosition);
}

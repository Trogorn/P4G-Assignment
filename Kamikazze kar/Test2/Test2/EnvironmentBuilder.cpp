#include "EnvironmentBuilder.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

void EnvironmentBuilder::BuildFloor()
{

	mQuad1 = new Model_Kami();

	mQuad1->Initialise(BuildQuad(mMeshMgr));
	MaterialExt mat = mQuad1->GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = mFX.mCache.LoadTexture("../building1.dds", true, gd3dDevice);
	mat.texture = "building1.dds";
	*mQuad1->GetPosition() = Vector3(0, 0, 0);
	*mQuad1->GetRotation() = Vector3(0, 0, 0);
	*mQuad1->GetScale() = Vector3(3, 1, 3);
	mQuad1->SetOverrideMat(&mat);


}

void EnvironmentBuilder::BuildAvoidedFloor()
{
}

void EnvironmentBuilder::BuildSkyscraper()
{
}

void EnvironmentBuilder::BuildTrees()
{
}

void EnvironmentBuilder::BuildObstacles()
{
}

void EnvironmentBuilder::RenderEnvironment()
{

	mFX.Render(*mQuad1, gd3dImmediateContext);

}

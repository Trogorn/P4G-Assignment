#include "Mesh.h"
#include "D3DUtil.h"
#include "FX.h"
#include "D3D.h"

void SubMesh::Release()
{
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB);
	mNumIndices = mNumVerts = 0;
}

Mesh& MeshManager::CreateMesh(const std::string& name)
{
	for (int i = 0; i < (int)mMeshes.size(); ++i)
		if (mMeshes[i]->mName == name)
		{
			DBOUT("Mesh name is duplicate: " << name);
			assert(false);
		}
	mMeshes.push_back(new Mesh(name));
	return *(mMeshes.back());
}

Mesh& MeshManager::GetMesh(const std::string& name)
{
	assert(!mMeshes.empty());
	for (int i = 0; i < (int)mMeshes.size(); ++i)
		if (mMeshes[i]->mName == name)
			return *mMeshes[i];
	DBOUT("Cannot find mesh: " << name);
	assert(false);
	return *mMeshes[0];
}


void MeshManager::Release()
{
	for (int i = 0; i < (int)mMeshes.size(); ++i)
		delete mMeshes[i];
	mMeshes.clear();
}

void Mesh::Release()
{
	for (int i = 0; i < (int)mSubMeshes.size(); ++i)
		delete mSubMeshes[i];
	mSubMeshes.clear();
}



void Mesh::CreateFrom(const VertexPosNormTex verts[], int numVerts, const unsigned int indices[], int numIndices, 
	const MaterialExt& mat, int meshStartIndex, int meshNumIndices)
{
	Release();
	SubMesh*p = new SubMesh;
	mSubMeshes.push_back(p);
	p->mNumIndices = meshNumIndices;
	p->mNumVerts = numVerts;
	p->material = mat;
	CreateVertexBuffer(sizeof(VertexPosNormTex)*numVerts, verts, p->mpVB);
	CreateIndexBuffer(sizeof(unsigned int)*numIndices, indices, p->mpIB);

}

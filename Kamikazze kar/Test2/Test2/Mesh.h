#ifndef MESH_H
#define MESH_H

#include <vector>

#include "ShaderTypes.h"


/*
Part of a vertex/index buffer that uses the same
material (colour and texture).
*/
class SubMesh
{
public:
	~SubMesh() {
		Release();
	}
	void Release();

	//buffer data
	ID3D11Buffer* mpVB = nullptr;
	ID3D11Buffer* mpIB = nullptr;
	int mNumIndices = 0;
	int mNumVerts = 0;

	MaterialExt material;
};

/*
A mesh contains all the vertex data for a 3D object
the indices give the triangle order. Where a mesh uses
different materials (colour or textures) it needs to be
rendered on material at a time, so the buffer is split
into different parts.
*/
class Mesh
{
public:
	Mesh(const std::string& name) : mName(name) {}
	~Mesh() { 
		Release(); 
	}
	void Release();
	void CreateFrom(const VertexPosNormTex verts[], int numVerts, const unsigned int indices[],
		int numIndices, const MaterialExt& mat, int meshStartIndex, int meshNumIndices);
	int GetNumSubMeshes() const {
		return (int)mSubMeshes.size();
	}
	SubMesh& GetSubMesh(int idx) {
		return *mSubMeshes.at(idx);
	}

	
	std::string mName;


private:
	Mesh(const Mesh& m) = delete;
	Mesh& operator=(const Mesh& m) = delete;

	std::vector<SubMesh*> mSubMeshes;
};

/*
A mesh is only ever loaded once, many model instances can use the same
mesh e.g. one tree mesh used to render 1000 trees, all different sizes,
orientations, positions.
*/
class MeshManager
{
public:
	~MeshManager() { 
		Release(); 
	}
	void Release();
	Mesh& GetMesh(const std::string& name);
	Mesh& CreateMesh(const std::string& name);

	std::vector<Mesh*> mMeshes;
};

#endif

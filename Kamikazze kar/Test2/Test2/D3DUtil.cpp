#include "D3DUtil.h"
#include "D3D.h"
#include <time.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

static float gTime = 0, gElapsedSec=0;

float GetClock()
{
	return gTime;
}

void AddSecToClock(float sec)
{
	gTime += sec;
	gElapsedSec = sec;
}


float GetElapsedSec()
{
	return gElapsedSec;
}

void CreateProjectionMatrix(Matrix& projM, float fieldOfView, float aspectRatio, float nearZ, float farZ)
{
	//  update the aspect ratio and recompute the projection matrix.
	projM = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ);
}

void CreateViewMatrix(DirectX::SimpleMath::Matrix& viewM, const DirectX::SimpleMath::Vector3& camPos, const DirectX::SimpleMath::Vector3& camTgt, const DirectX::SimpleMath::Vector3& camUp)
{
	viewM = XMMatrixLookAtLH(camPos, camTgt, camUp);
}

void CreateVertexBuffer(UINT bufferSize, const void *pSourceData, ID3D11Buffer* &pVB)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = bufferSize;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vinitData.pSysMem = pSourceData;
	HR(gd3dDevice->CreateBuffer(&vbd, &vinitData, &pVB));
}

void CreateIndexBuffer(UINT bufferSize, const void *pSourceData, ID3D11Buffer* &pIB)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = bufferSize;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = pSourceData;
	HR(gd3dDevice->CreateBuffer(&ibd, &iinitData, &pIB));
}

float GetRandom(float min, float max)
{
	float res = (float)rand() / RAND_MAX;
	res = min + res*(max - min);
	return res;
}

void SeedRandom(int seed /*= -1*/)
{
	if (seed == -1)
		seed = (int)time(nullptr);

	srand(seed);
}

Matrix InverseTranspose(const Matrix& m)
{
	XMMATRIX A = m;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

DirectX::SimpleMath::Vector4 Colours::GetRandomColour()
{
	switch (GetRandom(0, 6))
	{
	case 0:
		return Blue;
	case 1:
		return Cyan;
	case 2:
		return Green;
	case 3:
		return Magenta;
	case 4:
		return Red;
	case 5:
		return White;
	default:
		return Yellow;
	}
}

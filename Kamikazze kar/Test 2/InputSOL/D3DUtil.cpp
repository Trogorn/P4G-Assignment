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

void StripPathAndExtension(std::string& fileName, std::string* pPath/*=nullptr*/, std::string* pExt/*=nullptr*/)
{
	if (pPath)
		*pPath = "";
	if (pExt)
		*pExt = "";
	//filename only
	std::string::size_type n = fileName.find_last_of("\\/");
	if (n != std::string::npos)
	{
		if (pPath)
			pPath->append(fileName.c_str(), n + 1);
		fileName.erase(0, n + 1);
	}
	n = fileName.find_last_of(".");
	assert(n != std::string::npos);
	if (pExt)
		*pExt = fileName.substr(n);
	fileName.erase(n, fileName.length());
}


bool RayToSphere(const DirectX::SimpleMath::Vector3& centre, float radius, const DirectX::SimpleMath::Ray& ray, float& dist)
{
	//solve for tc
	//a vector from origin of ray to centre of sphere
	Vector3 toCentre(centre - ray.position);
	//how long is that vector
	float toCentreL = toCentre.Length();
	//how far along the ray until you are perpendicular to the centre of the sphere
	float tc = toCentre.Dot(ray.direction);
	//if negative we can't possibly hit it
	if (tc < 0.f)
		return false;

	//squared distance from centre of sphere to closest perpendicular point on ray
	float dSq = fabsf((tc*tc) - (toCentreL*toCentreL));
	//whats the radius we are checking against?
	float radiusSq = radius * radius;
	//too far away?
	if (dSq > radiusSq)
		return false;

	//how far back to step to reach the circumference of the sphere
	//this is the entry point, exit point is same distance in opposite direction
	float t1c = sqrtf(radiusSq - dSq);

	//solve for intersection point
	dist = tc - t1c;	

	return true;
}

bool RayToPlane(const DirectX::SimpleMath::Vector3& point, const DirectX::SimpleMath::Vector3& normal, const DirectX::SimpleMath::Ray& ray, float& dist)
{
	float denom = normal.Dot(ray.direction);
	if (fabsf(denom) > VERY_SMALL) 
	{
		dist = (point - ray.position).Dot(normal) / denom;
		if (dist >= 0) 
			return true; // you might want to allow an epsilon here too
	}
	return false;
}



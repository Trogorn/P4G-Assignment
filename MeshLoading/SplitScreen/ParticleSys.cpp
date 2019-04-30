#include <vector>

#include "ParticleSys.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "FX.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


const float Particle::GRAVITY = -9.8f;


//template for particle geometry - just a quad
const VertexParticle quad[] =
{	//quad in the XY plane
	{ Vector3(-0.5f, 0.5f, 0), Vector2(0, 0) },
	{ Vector3(0.5f, 0.5f, 0), Vector2(1, 0) },
	{ Vector3(0.5f, -0.5f, 0), Vector2(1, 1) },
	{ Vector3(-0.5f, -0.5f, 0), Vector2(0, 1) }
};




void ParticleSys::ReleasePS()
{
	mEmitters.clear();
	mParticles.clear();
	ReleaseCOM(pVBuffer);
	ReleaseCOM(pIBuffer);
}

void ParticleSys::InitialisePS()
{
	//particle cache
	Particle default;
	mParticles.insert(mParticles.begin(), MAX_PARTICLES, default);
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		if (i < MAX_PARTICLES - 1)
			mParticles[i].mpNext = &mParticles[i + 1];
		if (i > 0)
			mParticles[i].mpPrev = &mParticles[i - 1];
	}
	mpFreeList = &mParticles[0];
	mpBusyList = nullptr;
	
	//vertex buffer
	VertexParticle *vertices = new VertexParticle[MAX_PARTICLES * 4];
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		int offset = i * 4;
		memcpy(&vertices[offset], quad, sizeof(VertexParticle) * 4);
	}
	CreateVertexBuffer(sizeof(VertexParticle)*MAX_PARTICLES * 4, vertices, pVBuffer, true);
	delete[] vertices;

	//index buffer
	int nInd = MAX_PARTICLES * 2 * 3;
	unsigned int *indices = new unsigned int[nInd];
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		int iIdx = i * 2 * 3;
		int vIdx = i * 4;
		indices[iIdx++] = vIdx;
		indices[iIdx++] = vIdx + 1;
		indices[iIdx++] = vIdx + 2;

		indices[iIdx++] = vIdx;
		indices[iIdx++] = vIdx + 2;
		indices[iIdx++] = vIdx + 3;
	}
	CreateIndexBuffer(nInd * sizeof(unsigned int), indices, pIBuffer);
	delete[] indices;

	mEmitters.insert(mEmitters.begin(), MAX_EMITTERS, Emitter());
}

void ParticleSys::UpdateParticles(float dTime, const Vector3& camPos)
{
	for (auto& e : mEmitters)
		if (e.mAlive)
			e.Update(*this);
	
	if (!mpBusyList)
		return;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(gd3dImmediateContext->Map(pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	assert(mappedResource.pData);
	mNumParticles = 0;
	Particle *p = mpBusyList;
	while (p)
	{
		assert(p->mFlags&Particle::ALIVE);
		//simulate
		p->mPos += p->mVel * dTime;
		if (p->mFlags&Particle::USE_GRAVITY)
			p->mVel += Vector3(0, Particle::GRAVITY * dTime, 0);
		p->mLifeSec -= dTime;
		p->mAngle += p->mRotSpeed * dTime;
		//is it dead?
		if (p->mLifeSec <= 0)
		{
			Particle *pOld = p;
			p = p->mpNext;
			pOld->mFlags &= ~Particle::ALIVE;
			//cut out
			if (pOld->mpNext)
				pOld->mpNext->mpPrev = pOld->mpPrev;
			if (pOld->mpPrev)
				pOld->mpPrev->mpNext = pOld->mpNext;
			else
			{
				assert(mpBusyList == pOld);
				mpBusyList = pOld->mpNext;
			}
			//insert
			if (mpFreeList)
				mpFreeList->mpPrev = pOld;
			pOld->mpPrev = nullptr;
			pOld->mpNext = mpFreeList;
			mpFreeList = pOld;
		}
		else
		{
			//render it
			VertexParticle *pV = static_cast<VertexParticle *>(mappedResource.pData);
			int vIdx = mNumParticles * 4;
			Matrix mtx = Matrix::CreateRotationZ(p->mAngle);
			Matrix w = mtx * Matrix::CreateBillboard(p->mPos, camPos, FX::GetViewMatrix().Up(), &FX::GetViewMatrix().Forward());
			for (int i = 0; i < 4; ++i)
			{
				pV[vIdx + i].Pos = Vector3::Transform((quad[i].Pos * p->scale), w);
			}
			TexRect& tr = mSprites[p->mSpriteIdx];
			pV[vIdx].Tex = Vector2(tr.u, tr.v);
			pV[vIdx + 1].Tex = Vector2(tr.u + tr.w, tr.v);
			pV[vIdx + 2].Tex = Vector2(tr.u + tr.w, tr.v + tr.h);
			pV[vIdx + 3].Tex = Vector2(tr.u, tr.v + tr.h);
			mNumParticles++;
			p = p->mpNext;
		}
	}
	gd3dImmediateContext->Unmap(pVBuffer, 0);
}


void Emitter::Update(ParticleSys& ps)
{
	if (mData.mTotal == 0)
	{
		if (mData.mReleaseOnEmpty)
			StopAndRelease();
		return;
	}
	float elapsed = GetClock() - mLastEmitSec;
	if (elapsed < mEmitDelaySec)
		return;
	mLastEmitSec = GetClock();

	if (mData.mTotal == -1)
	{
		for (int i = 0; i < mData.mNumEmit; ++i)
			CreateParticle(ps);

	}
	else
	{
		int n = (mData.mTotal > mData.mNumEmit) ? mData.mNumEmit : mData.mTotal;
		for (int i = 0; i < n; ++i)
			if (!CreateParticle(ps))
				--n;
		mData.mTotal -= n;
	}
}

bool Emitter::CreateParticle(ParticleSys& ps)
{
	Particle *&pFreeList = ps.GetFreeList();
	if (!pFreeList)
		return false;
	//remove
	Particle *p = pFreeList;
	pFreeList = p->mpNext;
	if (pFreeList)
		pFreeList->mpPrev = nullptr;
	//insert
	Particle *&pBusyList = ps.GetBusyList();
	if (pBusyList)
		pBusyList->mpPrev = p;
	p->mpNext = pBusyList;
	p->mpPrev = nullptr;
	pBusyList = p;

	//initialise
	assert((p->mFlags&Particle::ALIVE) == false);
	p->mLifeSec = GetRandom(mData.mInitLifeSecMinMax.x, mData.mInitLifeSecMinMax.y);
	p->mPos = mData.mPos;
	p->mSpawnSec = GetClock();
	p->mSpriteIdx = mData.mSpriteIdx;

	switch (mData.mType)
	{
	case Type::OMNI:
		{
			p->mVel = Vector3(0, GetRandom(mData.mInitSpeedMinMax.x, mData.mInitSpeedMinMax.y), 0);
			Matrix mtx = Matrix::CreateRotationZ(GetRandom(0.f, 2*PI)) * Matrix::CreateRotationX(GetRandom(0.f,2*PI));
			p->mVel = Vector3::TransformNormal(p->mVel, mtx);
			break;
		}
	case Type::DIRECTION:
		{
			if (mData.mInitAngleMinMax.x == 0 && mData.mInitAngleMinMax.y == 0)
			{
				p->mVel = Vector3::Transform(Vector3(0, 1, 0), mData.mOrientation);
			}
			else
			{
				//normal random inside cone
				p->mVel = Vector3(0, 1, 0);
				Matrix mtx = Matrix::CreateRotationZ(GetRandom(mData.mInitAngleMinMax.x, mData.mInitAngleMinMax.y));
				p->mVel = Vector3::TransformNormal(p->mVel, mtx);
				mtx = Matrix::CreateRotationY(GetRandom(0.f, 2 * PI));
				p->mVel = Vector3::TransformNormal(p->mVel, mtx);
				p->mVel = Vector3::Transform(p->mVel, mData.mOrientation);
			}
			p->mVel *= GetRandom(mData.mInitSpeedMinMax.x, mData.mInitSpeedMinMax.y);
			break;
		}
	default:
		assert(false);
		break;
	}

	p->mFlags = mData.mParticleFlags | Particle::ALIVE;
	p->scale = GetRandom(mData.mScaleMinMax.x, mData.mScaleMinMax.y);

	p->mAngle = GetRandom(mData.mInitRotMinMax.x, mData.mInitRotMinMax.y);
	p->mRotSpeed = GetRandom(mData.mRotSpMinMax.x, mData.mRotSpMinMax.y);
	return true;
}


void ParticleSys::Render(FX::MyFX& fx)
{
	MaterialExt mat;
	mat.flags |= MaterialExt::TFlags::PARTICLE;
	mat.flags |= MaterialExt::TFlags::ADDITIVE;
	mat.flags &= ~MaterialExt::TFlags::ZTEST;
	mat.pTextureRV = fx.mCache.LoadTexture("particles.dds", true, gd3dDevice);

	if (mNumParticles>0)
		fx.RenderParticles(pVBuffer, pIBuffer, gd3dImmediateContext, 0, mNumParticles, Matrix::Identity, mat);

}
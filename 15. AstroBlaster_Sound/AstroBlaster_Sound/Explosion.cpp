// by Mark Featherstone (C) 2015 All Rights Reserved.
#include "CommonStates.h"
#include "Explosion.h"
#include "ModeGame.h"
#include "FX.h"
#include "ModeManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Explosion::Explosion() : mLightHdl(UDEF::MAX_UINT), mTimer(0), mScale(1)
{
	//done once at the start
	mSprite.Init(Vector2(), 0, 1, 0);
	FX::MyFX& fx = *FX::GetMyFX();
	ID3D11ShaderResourceView *p = fx.mCache.LoadTexture("explosion_opaque.dds", true, gd3dDevice);
	mSprite.Load(p, 23, Vector2(103, 103), 25);
	mSprite.Init(Vector2(52.5f, 52.5f), 0, 1, 0);
	Init();
}

void Explosion::Init()
{
	mSprite.Reset();
}

Explosion::~Explosion()
{
}

bool Explosion::GetIsPlaying() const
{
	return !mSprite.IsPaused();
}

int Explosion::GetNumPlays() const
{
	return mSprite.GetNumPlays();
}

void Explosion::SetSize( float sz )
{
	//scale up or down the explosion texture
	MPOD_ASSERT(sz>0);
	mSprite.SetScale(Vector2(sz,sz));
}

void Explosion::Play(const Vector2 &pos, bool lit, const std::string &sfx)
{
	//start it animating
	//try to get a light for it
	if(lit)
		mLightHdl=FX::GetMyFX()->mLightMgr.CachePointLight(1);
	//start the sfx playing

	mPos = pos;
	mTimer=0;
	mSprite.SetRotation(GetRandom(0.f, PI*2.f));
	mSprite.Play();
}

void Explosion::Update( float aTime )
{
	mSprite.Update(aTime);
	if (mSprite.GetNumPlays() >= 1)
		mSprite.Stop();
	if(mSprite.IsPaused() && mLightHdl!=UDEF::MAX_UINT)
	{
		//explosion is finished so release the light (we only have 8)
		FX::GetMyFX()->mLightMgr.ReleaseCachePointLight(mLightHdl);
	}
	else if(!mSprite.IsPaused())
	{
		//we want the explosion's light to expand rapidly and then contract
		mTimer+=aTime;
		float fadeUp=mSprite.GetTotalTime() * 1.5f;
		float fadeOut(fadeUp), full(fadeUp);
		fadeUp*=0.2f;
		fadeOut*=0.3f;
		mScale=mSprite.GetScale().x*250.f;
		if(mTimer<fadeUp)
			mScale*=mTimer/fadeUp;
		else if(mTimer>(full-fadeOut))
			mScale*=1.f - (mTimer-(full-fadeOut))/fadeOut;
	}
}

void Explosion::PostRender()
{
	if(mSprite.IsPaused())
		return;

	FX::MyFX& fx = *FX::GetMyFX();
	ID3D11ShaderResourceView *p = fx.mCache.LoadTexture("explosion_opaque.dds", true, gd3dDevice);
	Vector2 dim = fx.mCache.Get(p).dim;
	dim /= 2;

	Matrix world = Matrix::Identity;
	ModeGame *pG = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	assert(pG);
	Vector2 pos;
	Local3DtoScreenSpace(pos, Vector3(mPos.x, mPos.y, 0), world, pG->GetCamera().GetNearZ(), pG->GetCamera().GetFarZ());

	CommonStates state(gd3dDevice);
	fx.mpSpriteB->Begin(SpriteSortMode_Deferred, state.Additive());
	mSprite.Draw(fx.mpSpriteB, Vector2(pos.x, pos.y));

	fx.mpSpriteB->End();

	////if our light wasn't taken from us then update it to match the size of the explosion
	int lIdx;
	if (FX::GetMyFX()->mLightMgr.GetLightId(mLightHdl, lIdx))
		FX::SetupPointLight(lIdx, true, Vector3(mPos.x, mPos.y, 0), Vector3(1, 0.5f, 0), Vector3(0, 0, 0), Vector3(0.1f, 0.1f, 0.1f), mScale, 0.001f);
}


// by Mark Featherstone (C) 2015 All Rights Reserved.
#include "D3D.h"
#include "Bullet.h"
#include "ModeGame.h"
#include "ModeManager.h"
#include "ModeTitle.h"
#include "Bullet.h"
#include "Ship.h"
#include "FX.h"
#include "CommonStates.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

//load any resources we need, bullets are re-used at run time
Bullet::Bullet() : GameObject("BULLET"), mLife(0)
{
	SetVelocityLimit(1000);
}

Bullet::~Bullet()
{
}

//if finshed with the bullet, remove us from the game and turn off the light
void Bullet::Release()
{
	ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT_MSG(pGame,"Bad game mode");
	pGame->RemoveGameObject(this);
	FX::GetMyFX()->mLightMgr.ReleaseCachePointLight(mLightHdl);
}


void Bullet::Update(float aTime)
{
	// Call inherited method
	GameObject::Update(aTime);

	//don't live very long
	mLife-=aTime;
	if(mLife<=0)
	{
		Release();
		return;
	}

	//if we don't have a light, try and get one
	LightManager &lm=FX::GetMyFX()->mLightMgr;
	if(mLightHdl==UDEF::MAX_UINT)
		mLightHdl=lm.CachePointLight(0);
	int lightId;
	if(!lm.GetLightId(mLightHdl,lightId))
		mLightHdl=UDEF::MAX_UINT;
	//if we have a light, which get taken away from us at any time, update it
	if (mLightHdl != UDEF::MAX_UINT)
		FX::SetupPointLight(lightId, true, Vector3(GetPosition().x, GetPosition().y, 0), Vector3(0.4f, 0.4f, 1), Vector3(0, 0, 0), Vector3(0, 0, 1), 2500, 0.001f);
}

//we hit something, so play a sound and die
void Bullet::OnCollide(const GameObject* apOther)
{
	if(apOther->GetType()==mSpawnerType || apOther->GetType()==GetType())
		return;

	Release();
}

//bullets are re-used so we need to be able to reset them
void Bullet::Init()
{
	// Start pointing straight down the slope
	SetRotY(0.0f);
	SetRadius(15);
	SetPosition(Vector2(0,0));     
	ModeGame *p=dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT(p);
	SetCanCollide(true);
	mLife=1.1f;
	mLightHdl=FX::GetMyFX()->mLightMgr.CachePointLight(0);
}

//sprites get rendered after everything else as we can see through them
void Bullet::PostRender()
{
	Matrix world;
	GetModel().GetWorldMatrix(world);
	ModeGame *pG = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	assert(pG);
	Vector2 pos;
	Local3DtoScreenSpace(pos, Vector3(0,0,0), world, pG->GetCamera().GetNearZ(), pG->GetCamera().GetFarZ());

	FX::MyFX& fx = *FX::GetMyFX();

	CommonStates state(gd3dDevice);
	fx.mpSpriteB->Begin(SpriteSortMode_Deferred, state.Additive());
	ID3D11ShaderResourceView *pTex = fx.mCache.LoadTexture("blue_shot.dds", true, gd3dDevice);
	Vector3 dim(fx.mCache.Get(pTex).dim);
	dim /= 2;
	fx.mpSpriteB->Draw(pTex, pos, nullptr, Colours::White, UDEF::D2R(-GetRotY()-90),dim);

	fx.mpSpriteB->End();

}



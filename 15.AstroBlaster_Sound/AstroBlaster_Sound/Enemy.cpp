// by Mark Featherstone (C) 2013 All Rights Reserved.

// Application includes
#include "Enemy.h"
#include "ModeGame.h"
#include "ModeManager.h"
#include "ModeTitle.h"
//#include "Mesh.h"
#include "Bullet.h"
#include "Ship.h"
#include "Camera.h"
#include "GameObjectManager.h"
//#include "AudioMgr.h"
//#include "D3DManager.h"
#include "Explosion.h"
#include "LightManager.h"
#include "D3D.h"
#include "Mesh.h"
#include "FX.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy() :
	GameObject("ENEMY"), mHealth(10), mLifeSecs(0), mSfxHdl(UDEF::MAX_UINT)
{
	FX::MyFX& fx = *FX::GetMyFX();
	Mesh *mesh = GetMeshManager()->GetMesh("barrel");
	assert(mesh);
	GetModel().Initialise(*mesh);
}

Enemy::~Enemy()
{
	//delete mpExplosion;
}


void Enemy::Update(float aTime)
{
	// Call inherited method
	GameObject::Update(aTime);

	//spin
	mRot+=mRotInc * aTime;

	//update our internal clock
	mLifeSecs += aTime;


	//if the explosion runs it needs to update
	mExplosion.Update(aTime);
	if(!mExplosion.GetIsPlaying() && mExplosion.GetNumPlays()==1)
	{
		//when the explosion finishes we need to remove ourselves from the game
		ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
		MPOD_ASSERT_MSG(pGame,"Bad game mode");
		pGame->RemoveGameObject(this);
	}

	
}

void Enemy::TakeDamage(float damage, const GameObject *pOther)
{
	//get hold of the game mode, that must be active for us to be taking damage
	ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT(pGame);
	mHealth-=damage;
	//if we bounced off something then just make a noise
	if(mHealth>0 /*&& !GetIAudioMgr()->GetSfxMgr()->IsPlaying(mSfxHdl)*/)
	{
		//tiny impact so just make a noise
		//GetIAudioMgr()->GetSfxMgr()->Play("minorimpact",false,false,&mSfxHdl,QUIET_VOLUME);
		return;
	}

	//create an explosion effect, track the score and blow everything away with a force
	if(!mExplosion.GetIsPlaying())
	{
		mExplosion.Play(GetPosition(), true,"largeExplosion");
		pGame->ApplyForce(GetPosition(), GetRadius()*10, 1000, this);
		//the score is only valid if we were hit by a bullet
		if(pOther && pOther->GetType()==Bullet::CalcType())
			pGame->AddScore(100);
	}
	//we are dead so stop colliding
	SetCanCollide(false);
}

//we hit someone
void Enemy::OnCollide(const GameObject *pOther)
{
	ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT(pGame);

	//don't blow up if not on screen
	if(!pGame->GetCamera().SphereInFrustum(Vector3(GetPosition().x,GetPosition().y,0), GetRadius()))
		return;
	//work out how much damage to take
	//anything else hits us we're dead
	if(mLifeSecs<0.5f)
		return;
	TakeDamage(1,pOther);
}

//game objects are re-used at runtime, so we need this
//to make sure we reset appropriate variables
void Enemy::Init()
{
	mRot = 0;
	SetRotY(0.0f);
	SetPosition(Vector2(0,0));
	SetRadius(50);
	mRotInc = GetRandom(-1.f, 1.f);
	mLifeSecs=0;
	SetCanCollide(true);
	mExplosion.Init();
	SetVelocityLimit(300.0f);	//limit how fast things move, a bit like terminal velocity
	mHealth=10;
	mExplosion.SetSize(5);
}

void Enemy::PostRender()
{
	mExplosion.PostRender(); //explosions are transparent so need rendering after everything
}

void Enemy::Render()
{
	if(mExplosion.GetIsPlaying())
		return;	//if we are exploding don't render ourselves

	//build a matrix with the right scale for our size (we only have one model)
	float scale = 20;
	GetModel().GetScale() = Vector3(scale, scale, scale);
	GetModel().GetRotation().x = mRot;
	GetModel().GetPosition() = Vector3(GetPosition().x, GetPosition().y, 0);
	GameObject::Render();
}


void Enemy::AddForce( const Vector2 &force, float damage )
{
	//if we just spawned into the world ignore forces for a second
	if(mLifeSecs<1)
		return;
	//a force moves us and damages us
	SetVelocity(GetVelocity()+force);
	TakeDamage(damage);
}







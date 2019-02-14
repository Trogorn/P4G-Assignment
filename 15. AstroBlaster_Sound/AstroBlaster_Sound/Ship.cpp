// by Mark Featherstone (C) 2015 All Rights Reserved.

#include <math.h>
#include "d3d.h"

// Application includes
#include "ModeGame.h"
#include "ModeManager.h"
#include "ModeTitle.h"
#include "Ship.h"
#include "ShipController.h"
#include "Mesh.h"
#include "Model.h"
#include "Bullet.h"
#include "Asteroid.h"
#include "Warp.h"
#include "LightManager.h"
#include "WindowUtils.h"
#include "CommonStates.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "FX.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

const float Ship::MAX_TURN_SPEED = 200;

//allocate what we need, only done at the start during load
Ship::Ship() : GameObject("SHIP"),
	mThrustHdl(UDEF::MAX_UINT), mWaitingForRespawn(false), mController(ShipControlMethod::SCM_KEY)
{
	SetVelocityLimit(600);

	FX::MyFX& fx = *FX::GetMyFX();
	Mesh *mesh = GetMeshManager()->GetMesh("player");
	assert(mesh);
	GetModel().Initialise(*mesh);
	mThrust.Init(Vector2(64, 64), 0, 0.3f, 0);
	mThrust.Load(fx.mCache.LoadTexture("flames.dds", true, gd3dDevice), 16, Vector2(128, 128), 25);
}

void Ship::Update(float aTime)
{
	if(!mExplosion.GetIsPlaying() && !mWaitingForRespawn)
	{
		// Update inputs
		mController.Update(aTime);

		UpdateAngle(aTime);

		// keep moving and check for thrusting
		UpdatePosition(aTime);

		// Call inherited method
		GameObject::Update(aTime);
	}
	//if meant to be firing then try to let one go, the controller
	//sets the rate of fire - needs extracting
	if(mController.GetFiring() && !mExplosion.GetIsPlaying())
	{
		Bullet *pB;
		GET_NEXT_FREE_OBJ(pB,Bullet);
		if(pB)
		{
			//if we got a bullet then initialise it, play a sound and let it go
			ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
			MPOD_ASSERT_MSG(pGame,"Bad game mode");
			pGame->AddGameObject(pB);
			//which way are facing
			Vector2 dir(0,1);
			Matrix rot = Matrix::CreateRotationZ(UDEF::D2R(GetRotY()));
			Vector2::TransformNormal(dir, rot, dir);
			//spawn at nose of ship
			Vector2 pos(GetPosition());
			pos.x+=dir.x*100;
			pos.y+=dir.y*100;
			pB->SetPosition(pos);
			//make it move fast
			dir*=850 + GetVelocity().Length();
			pB->SetVelocity(dir);
			pB->SetSpawnerType(GetType());
			pB->SetRotY(GetRotY());
		}
	}

	////if we are exploding then that needs updating
	mExplosion.Update(aTime);
	if(mExplosion.GetNumPlays()==1)
	{
		//finished exploding so time to die
		mExplosion.Init();
		ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
		MPOD_ASSERT_MSG(pGame,"Bad game mode");
		int lives = pGame->GetLives();
		--lives;
		pGame->SetLives(lives);
		SetPosition(Vector2(0,0));
		if(lives>0)
		{
			//still have lives left so respawn
			mWaitingForRespawn=true;
			mRespawnTimer=-1;
		}
		else
		{
			//that's it, game over
			GetModeManager()->SwitchMode("GAMEOVER");
		}
	}
	//waiting to come back into the game
	if(mWaitingForRespawn)
	{
		mRespawnTimer+=aTime;
		ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
		MPOD_ASSERT_MSG(pGame,"Bad game mode");
		if(!pGame->IsInCollision(GetPosition(), GetRadius()*3,this,NULL))	
		{
			//there are no game objects anywhere near us, so add a nice warp effect
			//and let's get started
			Init();
			pGame->GetWarp().AddObject(*this);
		}
		else if(mRespawnTimer<0 || mRespawnTimer>0.5f)
		{
			//there are things in the way and they aren't getting out of the way
			//fast enough so blow them out of the way
			pGame->ApplyForce(Vector2(0,0),GetRadius()*10,1000,this);
			mRespawnTimer=0;
		}
	}

}

void Ship::OnCollide(const GameObject* apOther)
{
	//we don't collide with our own bullets 
	//we don't collide with anything if waiting to respawn
	if(apOther->GetType()==Bullet::CalcType() || mWaitingForRespawn)
		return;
	if(!mExplosion.GetIsPlaying())
	{
		//we aren't currently exploding and something hit us so go bang
		mExplosion.Play(GetPosition(),true,"largeExplosion");
		ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
		MPOD_ASSERT_MSG(pGame,"Bad game mode");
		pGame->ApplyForce(GetPosition(), mExplosion.GetSize(), 500, this);
	}
	//stop moving and stop colliding - it's all over
	SetCanCollide(false);
	SetVelocity(Vector2(0,0));
}

//if we are going to be reused we have to set things up properly each time
void Ship::Init()
{
	SetRotY(0.0f);
	SetRadius(75);

	SetCanCollide(true);
	SetVelocity(Vector2(0,0));

	// Call update angle with zero time step - it will set up the correct animation frame for us
	UpdateAngle(0.0f);
	mWaitingForRespawn=false;
	SetPosition(Vector2(0,0));

	//check we have enough bullets available
	if(GetGameObjectManager()->GetNum(Bullet::CalcType())==0)
		GetGameObjectManager()->Add(new Bullet, 20);

	mExplosion.Init();
	mExplosion.SetSize(5);
}


void Ship::PostRender()
{
	//transparent so render last
	
	if (mExplosion.GetIsPlaying())
	{
		mExplosion.PostRender();
		return;
	}

	Matrix world;
	GetModel().GetWorldMatrix(world);
	ModeGame *pG = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	assert(pG);
	Vector2 pos;
	Local3DtoScreenSpace(pos, Vector3(0, -6, 0), world, pG->GetCamera().GetNearZ(), pG->GetCamera().GetFarZ());

	FX::MyFX& fx = *FX::GetMyFX();

	CommonStates state(gd3dDevice);
	fx.mpSpriteB->Begin(SpriteSortMode_Deferred, state.Additive());

	if (!mThrust.IsPaused() && !mExplosion.GetIsPlaying() && !mWaitingForRespawn)
		mThrust.Draw(fx.mpSpriteB, Vector2(pos.x, pos.y)); 

	fx.mpSpriteB->End();
	
}

static unsigned int mLightHdl = UDEF::MAX_UINT;

void Ship::Render()
{
	if(mExplosion.GetIsPlaying() || mWaitingForRespawn)
		return;

	
	GetModel().GetRotation().x = -PI/2.f;
	GetModel().GetRotation().z = UDEF::D2R(GetRotY()) + PI;
	GetModel().GetScale() = Vector3(4.f, 4.f, 4.f);
	GetModel().GetPosition() = Vector3(GetPosition().x, GetPosition().y, 0);
	ResetStatesAfterSprites();
	GameObject::Render();

	FX::MyFX& fx = *FX::GetMyFX();
	LightManager &lm = FX::GetMyFX()->mLightMgr;
	if (mThrust.IsPaused())
	{
		if(mLightHdl!=UDEF::MAX_UINT)
			lm.ReleaseCachePointLight(mLightHdl);
	}
	else
	{
		Matrix world;
		GetModel().GetWorldMatrix(world);
		Vector3 off(0, 0, 40);
		Vector3::Transform(off, world, off);

		//if we don't have a light, try and get one
		if (mLightHdl == UDEF::MAX_UINT)
			mLightHdl = lm.CachePointLight(0);
		int lightId;
		if (!lm.GetLightId(mLightHdl, lightId))
			mLightHdl = UDEF::MAX_UINT;
		//if we have a light, which get taken away from us at any time, update it
		if (mLightHdl != UDEF::MAX_UINT)
			FX::SetupPointLight(lightId, true, off, Vector3(1, 0.3f, 0.3f), Vector3(0, 0, 0), Vector3(1, 0, 0), 1500, 0.001f);
	}
}

void Ship::UpdateAngle(float aTime)
{
	// Update the angle based on player input
	float lAngle = GetRotY();
	//we only turn at a fixed rate to keep it fair
	float lCurrentTurnSpeed = -mController.GetSteering() * MAX_TURN_SPEED;
	lAngle += lCurrentTurnSpeed * aTime;

	SetRotY(lAngle);
}

void Ship::UpdatePosition(float aTime)
{
	//if we are holding down thrust then display the effect but also
	//adjust our velocity so we actually move
	Vector2 vel(GetVelocity());
	if(mController.GetThrusting() || mController.GetReversing())
	{
		Vector2 force, dir(0, (mController.GetReversing()) ? -1.f : 1.f);
		Matrix rot = Matrix::CreateRotationZ(UDEF::D2R(GetRotY()));
		Vector2::TransformNormal(dir, rot, dir);

		force=dir*1750*aTime;
		vel+=force;
		if(mController.GetThrusting())
		{
			dir*=-1;
			mThrust.Play();
			mThrust.Update(aTime);
			mThrust.SetRotation( UDEF::D2R(-GetRotY() + 180) );


		}
	}	
	else
	{
		//not thrusting so slowdown, stop the effect and silence the sfx
		float alpha = 1.f - (2.f * aTime);
		vel*=(alpha<0)?0:alpha;
		mThrust.Stop();
	}

	SetVelocity(vel);
}


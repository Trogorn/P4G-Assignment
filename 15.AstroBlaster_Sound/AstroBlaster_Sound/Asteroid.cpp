// by Mark Featherstone (C) 2015 All Rights Reserved.

// Application includes
#include "Asteroid.h"
#include "ModeGame.h"
#include "ModeManager.h"
#include "ModeTitle.h"
#include "Bullet.h"
#include "Ship.h"
#include "Camera.h"
#include "GameObjectManager.h"
#include "Explosion.h"
#include "LightManager.h"
#include "FX.h"
#include "Mesh.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Asteroid::Asteroid() : 
	GameObject("ASTEROID"), mHealth(1), mTimeSecs(0), mHoming(false), 
		mSfxHdl(UDEF::MAX_UINT)
{
	//we need our model loading, but we don't own models, we just request them
	FX::MyFX& fx = *FX::GetMyFX();
	Mesh *mesh = GetMeshManager()->GetMesh("rock");
	assert(mesh);
	GetModel().Initialise(*mesh);
}

Asteroid::~Asteroid()
{
}


void Asteroid::Update(float aTime)
{
	// Call inherited method
	GameObject::Update(aTime);

	//spin
	mRot += mRotInc * aTime;

	//update our internal clock
	mTimeSecs+=aTime;

	//if the explosion runs it needs to update
	mExplosion.Update(aTime);
	if(!mExplosion.GetIsPlaying() && mExplosion.GetNumPlays()==1)
	{
		//when the explosion finishes we need to remove ourselves from the game
		ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
		MPOD_ASSERT_MSG(pGame,"Bad game mode");
		pGame->RemoveGameObject(this);
	}
	if(mHoming)
	{
		//?
	}
}

void Asteroid::TakeDamage(float damage, const GameObject *pOther)
{
	//get hold of the game mode, that must be active for us to be taking damage
	ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT(pGame);
	mHealth-=damage;
	//if we bounced off something then just make a noise
	if(mHealth>0)
	{
		//tiny impact so just make a noise
		return;
	}
	//create an explosion effect, track the score and blow everything away with a force
	if(!mExplosion.GetIsPlaying())
	{
		int score(0);
		float mag(0);
		switch(mAsteroidType)
		{
		case AsteroidType::SMALL:
			mExplosion.Play(GetPosition(), false,"smallExplosion");
			score=10;
			mag=125;
			break;
		case AsteroidType::MEDIUM:
			mExplosion.Play(GetPosition(), true,"medExplosion");
			score=50;
			mag=250;
			break;
		case AsteroidType::LARGE:
			mExplosion.Play(GetPosition(), true, "largeExplosion");
			score=100;
			mag=350;
			break;
		}
		pGame->ApplyForce(GetPosition(), GetRadius()*5, mag*2, this);
		//the score is only valid if we were hit by a bullet
		if(pOther && pOther->GetType()==Bullet::CalcType())
			pGame->AddScore(score);
	}
	//we are dead so stop colliding
	SetCanCollide(false);
	//if we are small then nothing left to do
	if(mAsteroidType==AsteroidType::SMALL)
		return;
	//spawn a random number of child asteroids
	int num = GetRandom(2, 4);
	for (int i = 0; i<num; ++i)
	{
		//get a nearby random position
		Vector2 pos(1,0);
		Matrix rot = Matrix::CreateRotationZ((float)GetRandom(0.f, PI*2.f));
		Vector2::Transform(pos, rot, pos);
		pos *= GetRandom(GetRadius()*0.25f, GetRadius());
		pos += GetPosition();
		//add a new asteroid of the appropriate type
		Asteroid *pA=pGame->AddAsteroid( (mAsteroidType==AsteroidType::LARGE)?AsteroidType::MEDIUM:AsteroidType::SMALL, pos, &GetPosition(), this);
	}
}

//we hit someone
void Asteroid::OnCollide(const GameObject *pOther)
{
	ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT(pGame);
	
	//don't blow up if not on screen
	if(!pGame->GetCamera().SphereInFrustum(Vector3(GetPosition().x,GetPosition().y,0), GetRadius()))
		return;
	//work out how much damage to take	
	if(pOther && pOther->GetType()==GetType())
	{
		//if its from another rock then just take a bit of damage 
		//provided we haven't just spawned
		if(mTimeSecs<1)
			return;
		const Asteroid *pA=dynamic_cast<const Asteroid*>(pOther);
		MPOD_ASSERT(pA);
		TakeDamage(GetDamage(pA),pOther);
	}
	else
	{
		//anything else hits us we're dead
		if(mTimeSecs<0.5f)
			return;
		if(pOther->GetType()==Ship::CalcType())
			TakeDamage(999,pOther);
		else
			TakeDamage(1,pOther);
	}
}

//game objects are re-used at runtime, so we need this
//to make sure we reset approriate variables
void Asteroid::Init()
{
	SetRotY(0.0f);
	SetPosition(Vector2(0,0));     
	
	//asteroids rotate randomly
	mRotInc = Vector2(GetRandom(0.25f, 0.75f), GetRandom(0.25f, 0.75f));
	
	mRot=Vector2(0,0);
	mTimeSecs=0;
	SetCanCollide(true);
	mHoming=false;
	mExplosion.Init();
	ModeGame* pGame = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT_MSG(pGame,"Bad game mode");
	SetVelocityLimit(200.0f);	//limit how fast things move, a bit like terminal velocity
}

void Asteroid::PostRender()
{
	mExplosion.PostRender(); //explosions are transparent so need rendering after everything
}

void Asteroid::Render()
{
	if(mExplosion.GetIsPlaying())
		return;	//if we are exploding don't render ourselves

	float scale(0.1f);
	switch(mAsteroidType)
	{
	case AsteroidType::LARGE:
		scale=0.1f;
		break;
	case AsteroidType::MEDIUM:
		scale=0.05f;
		break;
	case AsteroidType::SMALL:
		scale=0.025f;
		break;
	}
	GetModel().GetScale() = Vector3(scale, scale, scale);
	GetModel().GetRotation().x = mRot.x;
	GetModel().GetRotation().y = mRot.y;
	GetModel().GetPosition() = Vector3(GetPosition().x, GetPosition().y, 0);
	GameObject::Render();
}



float Asteroid::GetDamage( const Asteroid *pOther ) const
{
	//the bigger we are the less damage we take
	float damage;
	float weight;
	switch(mAsteroidType)
	{
	case AsteroidType::SMALL:
		weight=1;
		break;
	case AsteroidType::MEDIUM:
		weight=0.75f;
		break;
	case AsteroidType::LARGE:
		weight=0.5f;
		break;
	}
	//the bigger we are the more damage we do
	switch(pOther->mAsteroidType)
	{
	case AsteroidType::SMALL:
		damage=0.2f;
		break;
	case AsteroidType::MEDIUM:
		damage=0.4f;
		break;
	case AsteroidType::LARGE:
		damage=1.f;
		break;
	}

	return damage*weight;
}

//larger rocks have more health, bigger explosions
void Asteroid::SetType( AsteroidType type )
{
	switch(type)
	{
	case AsteroidType::LARGE:
		mHealth=3;
		SetRadius(100);
		mExplosion.SetSize(5);
		break;
	case AsteroidType::MEDIUM:
		mHealth=2;
		SetRadius(50);
		mExplosion.SetSize(2);
		break;
	case AsteroidType::SMALL:
		mHealth=1;
		SetRadius(25);
		mExplosion.SetSize(1);
		break;
	}
	mAsteroidType=type;
}

void Asteroid::AddForce(const Vector2 &force, float damage)
{
	//if we just spawned into the world ignore forces for a second
	if (mTimeSecs < 1)
		return;
	//a force moves us and damages us
	SetVelocity(GetVelocity() + force);
	TakeDamage(damage);
}





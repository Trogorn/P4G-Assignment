// by Mark Featherstone (C) 2015 All Rights Reserved.
#include <algorithm>

// Application includes
#include "Camera.h"
#include "GameObject.h"
#include "ModeGame.h"
#include "ModeGameOver.h"
#include "ModeManager.h"
#include "Asteroid.h"
#include "GameObjectManager.h"
#include "Bullet.h"
#include "Ship.h"
#include "Starfield.h"
#include "Warp.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "FX.h"
#include "CommonStates.h"

using namespace UDEF;
using namespace DirectX;
using namespace DirectX::SimpleMath;

const float ModeGame::HALF_WORLD = 1500;

ModeGame::ModeGame() : Mode("GAME"), mLevel(1), mMakeHoming(false)
{
	//game object array shouldn't be changing size (allocating)
	//so set a maximum size on them at the start
	mGameObjects.reserve(300);
	mWaitingForRemoval.reserve(300);
}

ModeGame::~ModeGame()
{
	//time to get rid of all game objects, but remember, we don't own them so just let them go
	for(GameObjects::iterator it=mGameObjects.begin(); it!=mGameObjects.end(); ++it)
		GetGameObjectManager()->Release(*it);
	mGameObjects.clear();
}



/*
see if a circle is in collision with any game objects
you can optionally specify two objects to ignore
bullets are also ignored
*/
bool ModeGame::IsInCollision(const Vector2 &centre, float radius, const GameObject *pIgnore, const GameObject *pIgnore2)
{
	int bullet=Bullet::CalcType();
	for(GameObjects::iterator it=mGameObjects.begin(); it!=mGameObjects.end(); ++it)
	{
		if((*it)==pIgnore || (*it)==pIgnore2 || !(*it)->GetCanCollide() || (*it)->GetType()==bullet)
		continue;
		float d2 = radius + (*it)->GetRadius();
		d2*=d2;
		Vector2 sep((*it)->GetPosition().x-centre.x, (*it)->GetPosition().y-centre.y);
		if(sep.LengthSquared()<d2)
			return true;
	}
	return false;
}

//wraps the above function to make it easier to use for a game object
//some game objects are spawned by others and the spawner is not to be
//collided with
bool ModeGame::IsInCollision(const GameObject &obj, const GameObject *pSpawner)
{
	if(!obj.GetCanCollide())
		return false;
	return IsInCollision(obj.GetPosition(), obj.GetRadius(), &obj, pSpawner);
}


void ModeGame::ApplyForce(const Vector2 &centre, float radius, float magnitude, GameObject *pIgnore)
{
	int bullet=Bullet::CalcType();
	for(GameObjects::iterator it=mGameObjects.begin(); it!=mGameObjects.end(); ++it)
	{
		//don't apply forces to non-colliders or bullets
		if(!(*it)->GetCanCollide() || (*it)==pIgnore || (*it)->GetType()==bullet)
			continue;
		//is it in range?
		float d2 = radius + (*it)->GetRadius();
		d2*=d2;
		Vector2 sep((*it)->GetPosition().x-centre.x, (*it)->GetPosition().y-centre.y);
		float sepD2 = sep.LengthSquared();
		MPOD_ASSERT_MSG(sepD2!=0,"Objects too close");
		if(sep.LengthSquared()>d2)
			continue;
		//create a vector to push the object - scaled by distance
		float l=sqrtf(sepD2);
		sep/=l;
		sep*=magnitude;
		//work out damage - scaled by distance
		float damage=magnitude/(l*1.5f);
		(*it)->AddForce(sep, damage);
	}
}

void ModeGame::ReleaseObject(GameObject &obj)
{
	GetGameObjectManager()->Release(&obj);
	GameObjects::iterator it2 = std::find(mGameObjects.begin(),mGameObjects.end(),&obj);	
	MPOD_ASSERT_MSG(it2!=mGameObjects.end(), "Cannot remove game object");
	mGameObjects.erase(it2);
}

//main update coordinates a lot of work
void ModeGame::Update(float aTime)
{
	//let everything move
	UpdateObjects(aTime);
	//see if anything is colliding
	CheckCollision();

	//safe to remove the dead
	for(GameObjects::iterator it=mWaitingForRemoval.begin(); it!=mWaitingForRemoval.end(); ++it)
		ReleaseObject(*(*it));
	mWaitingForRemoval.clear();

	// Update the camera
	mCamera.Update(aTime);

	//the background
	mStars.Update(aTime);
	mStars.SetCamPos(mpShip->GetPosition());

	////keep the music going

	mHud.Update(aTime);
	mWarp.Update(aTime);
}

void ModeGame::Render(float dTime)
{
	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.7f, 0.65f, 0.65f), Vector3(0.15f, 0.1f, 0.1f), Vector3(0.01f, 0.01f, 0.01f));

	BeginRender(Colours::Black);

	mCamera.PreRender();

	mStars.Render();

	FX::MyFX& fx = *FX::GetMyFX();

	for (int i = 0; i < (int)mGameObjects.size(); i++)
	{
		GameObject* lpGameObject = mGameObjects[i];
		lpGameObject->Render();
	}
	for (int i = 0; i < (int)mGameObjects.size(); i++)
	{
		GameObject* lpGameObject = mGameObjects[i];
		lpGameObject->PostRender();
	}

	mWarp.Render();

	mHud.Render();

	EndRender();
}


/*
Add an asteroid to the game - if there is a spare one available
type=small/med/large
desiredPos - where you want it, might not end up there due to collision
pMoveFrom - where it originally was meant to be, but was moved away due to collision
pSpawner - should be ignored during spawning, don't want to blow up instantly
spawnAnywhere - maybe we don't care where it spawns in
*/
Asteroid *ModeGame::AddAsteroid(AsteroidType type, const Vector2 &desiredPos, const Vector2 *pMoveFrom, GameObject *pSpawner, bool spawnAnywhere)
{
	Asteroid *pA;
	GET_NEXT_FREE_OBJ(pA,Asteroid);
	if(!pA)
		return NULL;
	pA->SetType(type);
	AddToWorld(pA,desiredPos,pMoveFrom,pSpawner,spawnAnywhere);
	return pA;
}

void ModeGame::AddToWorld(GameObject *pObj, const Vector2 &desiredPos, const Vector2 *pMoveFrom, GameObject *pSpawner, bool spawnAnywhere)
{
	mGameObjects.push_back(pObj);
	Vector2 vel(1,0), dir;
	if(pMoveFrom)
	{
		dir=Vector2(desiredPos.x-pMoveFrom->x, desiredPos.y-pMoveFrom->y);
		dir.Normalize();
	}
	else
	{
		Matrix mat = Matrix::CreateRotationZ(GetRandom(0.f, 2.f*PI));
		Vector2::TransformNormal(dir, mat, dir);
	}
	vel = dir * GetRandom(80.f, 160.f); 
	vel*=1.f + (float)mLevel/10.f;
	pObj->SetVelocity(vel);
	pObj->SetPosition(desiredPos);
	return;

	//find a reasonable place to put it
	Vector2 pos(desiredPos);
	int count(0);
	do 
	{
		if(spawnAnywhere)
		{
			pObj->SetPosition(Vector2(GetRandom(-HALF_WORLD,HALF_WORLD),GetRandom(-HALF_WORLD,HALF_WORLD)));
		}
		else
		{
			pObj->SetPosition(pos+dir);
			dir+=dir*pObj->GetRadius()*0.01f;
		}
		if(!IsInCollision(pObj->GetPosition(),pObj->GetRadius(),pSpawner,pObj))
			break;
	}while((++count)<10);
}



void ModeGame::Init()
{
	mHud.Init();
	if (GetGameObjectManager()->GetNum(Ship::CalcType()) == 0)
		GetGameObjectManager()->Add(new Ship, 1);
	GET_NEXT_FREE_OBJ(mpShip, Ship);
	mGameObjects.push_back(mpShip);
	if (GetGameObjectManager()->GetNum(Asteroid::CalcType()) == 0)
		GetGameObjectManager()->Add(new Asteroid, 100);
}

void ModeGame::OnKey(char key)
{
	switch (key)
	{
	case 27:
		GetModeManager()->SwitchMode("GAMEOVER");
		break;
	}
}


//the game is starting
void ModeGame::Enter()
{
	Mode::Enter();
	mStars.Load(Starfield::STARS);
	mLevel = 0;
	NextLevel();
	//// Make the ship the target of the camera
	mCamera.SetTarget(mpShip);
	mCamera.SetCamZ(-2000);
	mCamera.Update(0.0f);
	//mpCamera->SetupProjectionMatrix();
	mCamera.SetPositionLag(1.5f);
	mCamera.SettargetLag(1.5f);
	float limit = HALF_WORLD;// -900;
	mCamera.SetMoveLimits(Vector2(-limit,-limit), Vector2(limit,limit));

	mScore = 0;
	mLives = 3;
	mWarp.AddObject(*mpShip);
	mpShip->RespawnNow();
}

//game is exiting so tidy up
void ModeGame::Exit()
{
	Mode::Exit();

	for(GameObjects::iterator it=mGameObjects.begin();it!=mGameObjects.end();++it)
	{
		if((*it)==mpShip)
			continue;
		mWaitingForRemoval.push_back(*it);	
	}
	for(GameObjects::iterator it=mWaitingForRemoval.begin(); it!=mWaitingForRemoval.end(); ++it)
		ReleaseObject(*(*it));
	mWaitingForRemoval.clear();

	for (int i = 1; i < MAX_LIGHTS;++i)
		FX::DisableLight(i);
}


void ModeGame::RemoveGameObject( GameObject *pObj )
{
	GameObjects::iterator it=std::find(mWaitingForRemoval.begin(),mWaitingForRemoval.end(),pObj);
	MPOD_ASSERT_MSG(it==mWaitingForRemoval.end(),"Already waiting for removal");
	mWaitingForRemoval.push_back(pObj);
	pObj->SetCanCollide(false);
}

//don't let objects escape the bounds of the world
void ClampObjects(GameObject *pGameObject, int bulletType)
{
	Vector2 pos(pGameObject->GetPosition());
	if(pGameObject->GetType()==bulletType)
		return;
	if(pos.x>ModeGame::HALF_WORLD || pos.x<-ModeGame::HALF_WORLD || pos.y<-ModeGame::HALF_WORLD ||pos.y>ModeGame::HALF_WORLD)
	{
		Vector2 vel(pGameObject->GetVelocity());
		if(pos.x<-ModeGame::HALF_WORLD)
		{
			vel.x*=-1;
			pos.x=-ModeGame::HALF_WORLD;
		}
		else if(pos.x>ModeGame::HALF_WORLD)
		{
			vel.x*=-1;
			pos.x=ModeGame::HALF_WORLD;
		}
		if(pos.y<-ModeGame::HALF_WORLD)
		{
			vel.y*=-1;
			pos.y=-ModeGame::HALF_WORLD;
		}
		else if(pos.y>ModeGame::HALF_WORLD)
		{
			vel.y*=-1;
			pos.y=ModeGame::HALF_WORLD;
		}
		pGameObject->SetPosition(pos);
		//give it a big jolt in the correct direction
		vel.Normalize();
		vel *= 1000.f;
		pGameObject->SetVelocity(vel);
	}
}

//work out how many asteroids are left in the world
void CheckAsteroid(GameObject *pGameObject, bool makeHoming, int &nLarge, int&nMed, int &nSmall)
{
	Asteroid *pA=dynamic_cast<Asteroid*>(pGameObject);
	MPOD_ASSERT(pA);
	if(makeHoming)
		pA->SetHoming(true);
	switch(pA->GetAsteroidType())
	{
	case AsteroidType::SMALL:
		++nSmall;
		break;
	case AsteroidType::MEDIUM:
		++nMed;
		break;
	case AsteroidType::LARGE:
		++nLarge;
		break;
	}
}

void ModeGame::UpdateObjects( float aTime )
{
	int bulletType = Bullet::CalcType();
	int asteroidType=Asteroid::CalcType();
	int nLarge(0), nMed(0), nSmall(0);
	//// Update all game objects
	for (int i = 0; i < (int)mGameObjects.size(); i++)
	{
		//main object update
		GameObject* pGameObject = mGameObjects[i];
		pGameObject->Update(aTime);
		
		if(pGameObject->GetType()==asteroidType)
			CheckAsteroid(pGameObject, mMakeHoming,nLarge,nMed,nSmall);
		ClampObjects(pGameObject, bulletType);
	}
	if((nSmall+nLarge+nMed)==0)
	{
		//no asteroids - woO!
		NextLevel();
	}
	else if(nLarge==0 && nMed==0 && !mMakeHoming)
	{
		//only little ones left, stop it turning into a chore by making them chase the player
		mMakeHoming=true;
	}
}

void ModeGame::NextLevel()
{
	++mLevel;
	mMakeHoming=false;
	//create a new set of large asteroids
	for(int i=0; i<1+mLevel*2; ++i)
	{
		Vector2 pos(GetRandom(-HALF_WORLD,HALF_WORLD), GetRandom(-HALF_WORLD,HALF_WORLD));
		Asteroid *p = AddAsteroid(AsteroidType::LARGE,pos,NULL,NULL,true);
		if(p)
			mWarp.AddObject(*p);
	}
}

void ModeGame::CheckCollision()
{
	// Perform collision checks on all game objects
	for(GameObjects::iterator it=mGameObjects.begin(); it!=(mGameObjects.end()-1); ++it)
	{
		GameObject *pFirst = *it;
		if(!pFirst->GetCanCollide())
			continue;
		for(GameObjects::iterator it2=(it+1); it2!=mGameObjects.end(); ++it2)
		{
			GameObject *pSecond = *it2;
			if(!pSecond->GetCanCollide())
				continue;
			//cricle to circle distance check
			float d2 = pFirst->GetRadius() + pSecond->GetRadius();
			d2*=d2;
			Vector2 sep(pSecond->GetPosition().x-pFirst->GetPosition().x, pSecond->GetPosition().y-pFirst->GetPosition().y);
			float sepDist2=sep.LengthSquared(); 
			if(sepDist2>d2)
				continue;
			//they've hit!!
			pFirst->OnCollide(pSecond);
			pSecond->OnCollide(pFirst);
			//exchange momentum
			Vector2 vel1, vel2;
			if(pFirst->GetResultantMomentum(vel1,pSecond) && 
				pSecond->GetResultantMomentum(vel2,pFirst))
			{
				//push apart
				sep/=sqrtf(sepDist2);
				pSecond->SetPosition(pSecond->GetPosition()+sep*pSecond->GetRadius());
				pFirst->SetPosition(pFirst->GetPosition()-sep*pFirst->GetRadius());
				//exchange
				pSecond->SetVelocity(vel1+sep*50.f);
				pFirst->SetVelocity(vel2-sep*50.f);
			}
			//collision may mean no further tests are necessary
			if(!pFirst->GetCanCollide())
				break;
		}
	}
}

void ModeGameOver::OnKey(char key)
{
	switch (key)
	{
	case 27:
		GetModeManager()->SwitchMode("TITLE");
		break;
	}
}

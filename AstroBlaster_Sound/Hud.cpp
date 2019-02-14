// by Mark Featherstone (C) 2015 All Rights Reserved.
#include <string>
#include <sstream>

#include "Hud.h"
#include "ModeManager.h"
#include "ModeGame.h"
#include "Camera.h"
#include "Ship.h"

#include "CommonStates.h"
#include "FX.h"
#include "WindowUtils.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;



void Hud::Init()
{
	mInitialised=true;

	//how many lives do we start with, we can use this to spot when
	//the player loses one
	ModeGame *p=dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT_MSG(p,"Bad mode");
	mLives=p->GetLives();
}

void Hud::Update(float aTime)
{
	//spot when a life is lost - would be better as an event driven system
	ModeGame *p=dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	MPOD_ASSERT_MSG(p,"Bad mode");
	if(mLives!=p->GetLives())
	{
		//life is lost so flash the remaining lives
		mLives=p->GetLives();
		mFlash=3;
		mVisible=false;
		mTimer=0.25f;
	}
	mTimer-=aTime;
}


void Hud::Render()
{
	//render everything in the right order
	MPOD_ASSERT_MSG(mInitialised,"Not initialised");


}

Hud::Hud() : mInitialised(false), mLives(0), mVisible(true), mTimer(0), mFlash(0)
{
}

Hud::~Hud()
{
}

// by Mark Featherstone (C) 2015 All Rights Reserved.

#include <string>
#include <sstream>

// Application includes
#include "ModeGameOver.h"
#include "ModeTitle.h"
#include "ModeManager.h"
#include "ModeGame.h"
#include "Starfield.h"
#include "Asteroid.h"
#include "UserFolder.h"
#include "File.h"
#include "FX.h"
#include "CommonStates.h"
#include "WindowUtils.h"
#include "Mesh.h"

using namespace std;
using namespace UDEF;
using namespace DirectX;
using namespace DirectX::SimpleMath;


ModeGameOver::ModeGameOver() : Mode("GAMEOVER"), mRot(0)
{
}

ModeGameOver::~ModeGameOver()
{
}


void ModeGameOver::Update(float aTime)
{
	////keep the music playing
}

void ModeGameOver::Render(float dTime)
{
	BeginRender(Colours::Black);

	mCamera.PreRender();

	mStars.Render();

	FX::MyFX& fx = *FX::GetMyFX();

	ResetStatesAfterSprites();
	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.7f, 0.65f, 0.65f), Vector3(0.15f, 0.1f, 0.1f), Vector3(1, 1, 1));

	CommonStates state(gd3dDevice);
	fx.mpSpriteB->Begin(SpriteSortMode_Deferred, state.NonPremultiplied());


	int w, h;
	GetClientExtents(w, h);


	fx.mpSpriteB->End();

	EndRender();
}

void ModeGameOver::Enter()
{
	Mode::Enter();
	mCamera.SetPosition(Vector2(0, 0));
	FX::MyFX& fx = *FX::GetMyFX();
	mpTitleTex = fx.mCache.LoadTexture("gameover.dds", true, gd3dDevice);
	mStars.Load(Starfield::PLASMA);

	
	

}



void ModeGameOver::Exit()
{
	Mode::Exit();
}


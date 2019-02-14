// by Mark Featherstone (C) 2015 All Rights Reserved.

#include <thread>

#include "CommonStates.h"


#include "D3D.h"
#include "FX.h"
#include "Mesh.h"
#include "Model.h"
#include "ModeTitle.h"
#include "ModeManager.h"
#include "Camera.h"
#include "FX.h"
#include "WindowUtils.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

extern bool g_okToQuit;

void ResetMaterials(Mesh& ms, const Vector4& diff, const Vector4& amb, const Vector4& spec)
{
	for (int i = 0; i < ms.GetNumSubMeshes(); ++i)
	{
		MaterialExt& mat = ms.GetSubMesh(i).material;
		mat.gfxData.Set(diff, amb, spec);
	}
}


ModeTitle::ModeTitle() : Mode("TITLE"), mLoadFinished(false)
{
}


ModeTitle::~ModeTitle()
{
}

void ModeTitle::Update(float aTime)
{
	mStars.SetCamPos(Vector2(GetClock()*10,0));
	mStars.Update(aTime);

	////keep the music playing
}

void ModeTitle::Render(float dTime)
{
	BeginRender(Colours::Black);

	mCamera.PreRender();

	mStars.Render();

	FX::MyFX& fx = *FX::GetMyFX();

	ResetStatesAfterSprites();
	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.7f, 0.65f, 0.65f), Vector3(0.15f, 0.1f, 0.1f), Vector3(0.01f, 0.01f, 0.01f));


	CommonStates state(gd3dDevice);
	fx.mpSpriteB->Begin(SpriteSortMode_Deferred, state.NonPremultiplied());


	//background
	int w, h;
	GetClientExtents(w, h);
	float sz(h / mTitleDim.y);
	if (sz > 1.25f )
		sz = 1.25f;
	fx.mpSpriteB->Draw(mpTitleTex, Vector2(w / 2.f, h / 2.f), nullptr, Colours::White, 0, mTitleDim*0.5f, Vector2(sz,sz));



	fx.mpSpriteB->End();

	if (mLoadThread.valid() && mLoadThread._Is_ready())
	{
		mLoadThread.get();
		mLoadFinished = true;
	}

	EndRender();
}

void ModeTitle::Enter()
{
	Mode::Enter();
	mCamera.SetPosition(Vector2(0,0));

}

void ModeTitle::Exit()
{
	Mode::Exit();
}

void ModeTitle::Load()
{
	//load the title as fast as possible
	FX::MyFX& fx = *FX::GetMyFX();

	mpTitleTex = fx.mCache.LoadTexture("title.dds", true, gd3dDevice);
	mTitleDim = fx.mCache.Get(mpTitleTex).dim;
	mpLoadingTex = fx.mCache.LoadTexture("loading.dds", true, gd3dDevice);
	mLoadingDim = fx.mCache.Get(mpLoadingTex).dim;
	mStars.Load(Starfield::PLASMA);
	Mesh* ms = &GetMeshManager()->CreateMesh("player");
	ms->CreateFrom("../bin/data/fighter/space_frigate_6.FBX", gd3dDevice, FX::GetMyFX()->mCache);
	ResetMaterials(*ms, Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 0), Vector4(0.07f, 0.05f, 0.05f, 50));

	//start all other resources loading
	mLoadThread = std::async(launch::async, &ModeTitle::LoadWorker, this);

}

void ModeTitle::OnKey(char key)
{
	if (!mLoadFinished)
		return;
	switch (key)
	{
	case 27:
		PostQuitMessage(0);
		break;
	case 'z':
	case 'Z':
		if (mLoadFinished)
			GetModeManager()->SwitchMode("GAME");
		break;
	}
}


void ModeTitle::LoadWorker()
{
	Mesh* ms = &GetMeshManager()->CreateMesh("rock");
	ms->CreateFrom("../bin/data/asteroid1.x", gd3dDevice, FX::GetMyFX()->mCache);
	ResetMaterials(*ms, Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 0), Vector4(0.05f, 0.05f, 0.05f, 5));
	ms = &GetMeshManager()->CreateMesh("barrel");
	ms->CreateFrom("../bin/data/barrel/barrel_unsmoothed.FBX", gd3dDevice, FX::GetMyFX()->mCache);
	ResetMaterials(*ms, Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 0), Vector4(0.05f, 0.07f, 0.05f, 50));

	FX::MyFX& fx = *FX::GetMyFX();
	fx.mCache.LoadTexture("bullet.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("blue_shot.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("explosion_opaque.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("flames.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("radar.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("ship_icon.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("starfield_noalpha.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("starscape4.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("sweep.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("warp.dds", true, gd3dDevice);
	fx.mCache.LoadTexture("gameover.dds", true, gd3dDevice);

	GetModeManager()->Init();
}

const float ModeTitle::LOAD_DELAY_SECS = 2;


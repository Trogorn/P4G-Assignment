// by Mark Featherstone (C) 2015 All Rights Reserved.


#include "CommonStates.h"

#include "D3D.h"
#include "Warp.h"
#include "GameObject.h"
#include "SpriteBatch.h"
#include "FX.h"
#include "Camera.h"
#include "ModeGame.h"
#include "ModeManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Warp::Warp()
{
	//create our warp data - only 20 objects can warp in at once, should test that
	mObjects.insert(mObjects.begin(),20,Data());
}

void Warp::AddObject( GameObject &obj )
{
	//find a free warp data item
	for(Objects::iterator it=mObjects.begin();it!=mObjects.end();++it)
	{
		if((*it).mInUse)
			continue;
		//set it up and start it animating
		(*it).mInUse=true;
		(*it).pObj=&obj;
		(*it).mTime=0;
		(*it).mMode=Data::FADE_IN;

		return;
	}
}

void Warp::Update( float aTime )
{
	for(Objects::iterator it=mObjects.begin();it!=mObjects.end();++it)
	{
		//find objects animating
		if(!(*it).mInUse)
			continue;
		//update their clock and stop them if too old
		(*it).mTime+=aTime;
		if((*it).mTime>2)
			(*it).mInUse=false;
	}
}

void Warp::Render()
{

	FX::MyFX& fx = *FX::GetMyFX();

	CommonStates state(gd3dDevice);
	fx.mpSpriteB->Begin(SpriteSortMode_Immediate, state.Additive(), state.AnisotropicWrap());

	for(Objects::iterator it=mObjects.begin();it!=mObjects.end();++it)
	{
		//find objects animating
		if(!(*it).mInUse)
			continue;
		//fix the warp effect to the object
		Vector2 pos = (*it).pObj->GetPosition();
		Matrix world;
		(*it).pObj->GetModel().GetWorldMatrix(world);
		ModeGame *pG = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
		assert(pG);
		Local3DtoScreenSpace(pos, Vector3(0,0,0), world, pG->GetCamera().GetNearZ(), pG->GetCamera().GetFarZ());

		//fade it in and out over time
		float sz=(*it).pObj->GetRadius()*3;
		switch((*it).mMode)
		{
		case Data::FADE_IN:
			sz=sz*(1.f+(*it).mTime);
			if((*it).mTime>=1)
				(*it).mMode=Data::FADE_OUT;
			break;
		case Data::FADE_OUT:
			sz=sz*2*(1.f-((*it).mTime-1.f));
			break;
		}
		//draw two textures, spin them in opposite directions
		ID3D11ShaderResourceView* p = fx.mCache.LoadTexture("warp.dds", true, gd3dDevice);
		Vector2 dim = fx.mCache.Get(p).dim;
		dim /= 2;
		fx.mpSpriteB->Draw(p, pos, nullptr, Colours::White, GetClock(), dim, sz*0.0015f);
		fx.mpSpriteB->Draw(p, pos, nullptr, Colours::White, -GetClock(), dim, sz*0.0015f);

	}

	fx.mpSpriteB->End();
}

Warp::~Warp()
{
}

#include "CommonStates.h"


#include "D3D.h"
#include "FX.h"
#include "Starfield.h"
#include "WindowUtils.h"
#include "ModeGame.h"
#include "ModeManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Starfield::Update(float aTime)
{
	switch (mMode)
	{
	case STARS:
		mBarrierTB.Update(aTime);
		mBarrierLR.Update(aTime);
		break;
	}
}

void Starfield::Render()
{
	FX::MyFX& fx = *FX::GetMyFX();

	CommonStates state(gd3dDevice);
	fx.mpSpriteB->Begin(SpriteSortMode_Immediate, state.Additive(), state.AnisotropicWrap());

	switch (mMode)
	{
	case PLASMA:
		RenderPlasma();
		break;
	case STARS:
		RenderStars();
		break;
	}
	fx.mpSpriteB->End();
}

void Starfield::RenderPlasma()
{
	FX::MyFX& fx = *FX::GetMyFX();
	int w, h;
	float xs, ys;
	RECT dest, source;

	GetClientExtents(w, h);
	fx.mpSpriteB->Draw(mpBack, Vector2(w / 2.f, h / 2.f), nullptr, Colours::White, PI + GetClock()*-0.2f, Vector2(mBackDim.x / 2.f, mBackDim.y / 2.f), 1.5f);
	fx.mpSpriteB->Draw(mpBack, Vector2(w / 2.f, h / 2.f), nullptr, Colours::White, GetClock()*0.1f, Vector2(mBackDim.x / 2.f, mBackDim.y / 2.f), 1.25f);

	xs = w / mFrontDim.x;
	ys = h / mFrontDim.y;
	dest = RECT{ 0, 0, (int)(mFrontDim.x*xs), (int)(mFrontDim.y*xs) };
	source = RECT{ (INT)mPos.x, (int)mPos.y, (int)(mFrontDim.x*xs + mPos.x), (int)(mFrontDim.y*xs + mPos.y) };
	fx.mpSpriteB->Draw(mpFront, dest, &source, Colours::White, 0, Vector2(0,0), DirectX::SpriteEffects_None,0);

}

void Starfield::RenderStars()
{
	FX::MyFX& fx = *FX::GetMyFX();

	int w, h;
	GetClientExtents(w, h);

	Vector2 motionNorm{ mPos.x / ModeGame::HALF_WORLD, mPos.y / ModeGame::HALF_WORLD }, motion;
	float maxOffset = mBackDim.x * 0.125f;
	motion = motionNorm * Vector2(-maxOffset, maxOffset);
	RECT dest{ 0, 0, w, h }, source{ (int)(-maxOffset - motion.x), (int)(-maxOffset - motion.y), (int)(mBackDim.x + maxOffset - motion.x), (int)(mBackDim.y + maxOffset - motion.y) };
	fx.mpSpriteB->Draw(mpBack, dest, &source, Colours::White, 0, Vector2(0, 0));



	RenderBoundary();
}


void Starfield::RenderBoundary()
{
	FX::MyFX& fx = *FX::GetMyFX();

	//boundary
	ModeGame* pG = dynamic_cast<ModeGame*>(GetModeManager()->GetMode("GAME"));
	assert(pG);
	Matrix world = Matrix::Identity;
	Vector2 tl;
	Local3DtoScreenSpace(tl, Vector3(-ModeGame::HALF_WORLD, ModeGame::HALF_WORLD, 0), world, pG->GetCamera().GetNearZ(), pG->GetCamera().GetFarZ());
	Vector2 br;
	Local3DtoScreenSpace(br, Vector3(ModeGame::HALF_WORLD, -ModeGame::HALF_WORLD, 0), world, pG->GetCamera().GetNearZ(), pG->GetCamera().GetFarZ());

	ID3D11ShaderResourceView* pTex = fx.mCache.LoadTexture("lightning.dds", true, gd3dDevice);
	Vector2 dim = fx.mCache.Get(pTex).dim;
	dim.x /= 8;
	float scale = ModeGame::HALF_WORLD / dim.y;

	mBarrierLR.Play();
	RECT left{ (int)(tl.x - dim.x/2), (int)tl.y, (int)(tl.x+dim.x/2), (int)br.y };
	mBarrierLR.Draw(fx.mpSpriteB, left, Vector2(1,scale));
	RECT right{ (int)(br.x - dim.x / 2), (int)tl.y, (int)(br.x + dim.x / 2), (int)br.y };
	mBarrierLR.Draw(fx.mpSpriteB, right, Vector2(1, scale));

	mBarrierTB.Play();
	RECT top{ (int)(tl.x + dim.x / 2), (int)(tl.y - dim.x / 2), (int)(br.x - dim.x / 2), (int)(tl.y + dim.x / 2) };
	mBarrierTB.Draw(fx.mpSpriteB, top, Vector2(scale,1));
	RECT bottom{ (int)(tl.x + dim.x / 2), (int)(br.y - dim.x / 2), (int)(br.x - dim.x / 2), (int)(br.y + dim.x / 2) };
	mBarrierTB.Draw(fx.mpSpriteB, bottom, Vector2(scale,1));
}

void Starfield::Load(Mode mode)
{
	mMode = mode;
	FX::MyFX& fx = *FX::GetMyFX();
	switch (mode)
	{
	case PLASMA:
		mpBack = fx.mCache.LoadTexture("red.dds", true, gd3dDevice);
		mBackDim = fx.mCache.Get(mpBack).dim;
		mpFront = fx.mCache.LoadTexture("white.dds", true, gd3dDevice);
		mFrontDim = fx.mCache.Get(mpFront).dim;
		break;
	case STARS:
		mpBack = fx.mCache.LoadTexture("starscape2.dds", true, gd3dDevice);
		mBackDim = fx.mCache.Get(mpBack).dim;
		mpFront = fx.mCache.LoadTexture("starfield_noalpha.dds", true, gd3dDevice);
		mFrontDim = fx.mCache.Get(mpFront).dim;
		mBarrierLR.Init(Vector2(0, 0), 0, 1, 0);
		ID3D11ShaderResourceView* pTex = fx.mCache.LoadTexture("lightning.dds", true, gd3dDevice);
		mBarrierLR.Load(pTex, 8, Vector2(128,512), 30);
		mBarrierTB.Init(Vector2(0, 0), 0, 1, 0);
		pTex = fx.mCache.LoadTexture("lightning2.dds", true, gd3dDevice);
		mBarrierTB.Load(pTex, 8, Vector2(512, 128), 30);
		break;
	}
}

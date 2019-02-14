#ifndef STARFIELD_H
#define STARFIELD_H

#include "D3D.h"
#include "SpriteAnim.h"

/*
A parallax 2 layer scrolling star field that helps
create the illusion of movement
*/
class Starfield 
{
public:
	typedef enum { PLASMA, STARS } Mode;
	void Update(float dTime);
	//Work out where we are meant to be looking the world so
	//we can scroll the background
	void SetCamPos(const DirectX::SimpleMath::Vector2 &pos) {
		mPos = pos;
	};
	//convert movement in the world to a movement of the 
	//star field textures
	void Render();
	void Load(Mode mode);

private:
	DirectX::SimpleMath::Vector2 mPos;	//track the position of the world camera and scroll against it
	ID3D11ShaderResourceView *mpBack = nullptr;
	DirectX::SimpleMath::Vector2 mBackDim, mFrontDim;
	ID3D11ShaderResourceView *mpFront = nullptr;
	Mode mMode = PLASMA;
	SpriteAnim mBarrierTB, mBarrierLR;

	void RenderPlasma();
	void RenderStars();
	void RenderBoundary();
};
#endif

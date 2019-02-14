// by Mark Featherstone (C) 2015 All Rights Reserved.
#ifndef MODEGAMEOVER_H
#define MODEGAMEOVER_H

#include "Camera.h"
#include "Mode.h"
#include "Starfield.h"

class Asteroid;

/*
Game over mode shows the score and the best ever score
The message changes if you just got that best ever score
*/
class ModeGameOver : public Mode
{
public:
	ModeGameOver();
	~ModeGameOver();

	void Update(float aTime) override;
	void Render(float dTime) override;
	void Enter() override;
	void Exit() override;
	void OnKey(char key) override;

private:
	ID3D11ShaderResourceView *mpTitleTex = nullptr;
	Starfield mStars;
	Camera mCamera;		//need a camera to show a big asteroid in the background
	unsigned int mMusicHdl;	//background music
	float mRot;				//spin the asteroid	
	int mLastHighScore;		//how far in the game did we get
	int mLastHighLevel;
};

#endif
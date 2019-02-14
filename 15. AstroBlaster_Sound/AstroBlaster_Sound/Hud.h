// by Mark Featherstone (C) 2015 All Rights Reserved.
#ifndef HUD_H
#define HUD_H


/*
Head Up Display - all the 2d overlay bits on the
main game screen (radar, score, lives, etc)
*/
class Hud
{
public:
	Hud();
	~Hud();
	void Update(float dTime);
	void Render();
	void Init();	
private:
	bool mInitialised;	//everything setup OK?
	int mLives;			//number of lives left
	int mFlash;			//do we want lives to flash because they just lost one?
	float mTimer;		//flashing needs timing
	bool mVisible;		//shall we bother to render the hud at all

};


#endif
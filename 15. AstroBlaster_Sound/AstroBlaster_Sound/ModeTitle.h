// by Mark Featherstone (C) 2015 All Rights Reserved.

#ifndef MODETITLE_H
#define MODETITLE_H

#include <future>

// Application includes
#include "Mode.h"
#include "UtilsDefines.h"
#include "Camera.h"
#include "Starfield.h"


/*
The title screen, explains the controls and loads the rest of the game
*/
class ModeTitle : public Mode
{
public:
	ModeTitle();
	~ModeTitle();

	void Update(float aTime) override;
	void Render(float dTime) override;
	void Enter() override;
	void Exit() override;
	void OnKey(char key) override;

	void Load();

private:
	std::future<void> mLoadThread;
	void LoadWorker();

	ID3D11ShaderResourceView *mpTitleTex = nullptr;
	DirectX::SimpleMath::Vector2 mTitleDim, mLoadingDim;
	ID3D11ShaderResourceView *mpLoadingTex = nullptr;
	static const float LOAD_DELAY_SECS;
	Starfield mStars;
	Camera mCamera;		//fixed camera
	unsigned int mMusicHdl = UDEF::MAX_UINT;	//keep the music playing
	
	bool mLoadFinished=false;
};

#endif
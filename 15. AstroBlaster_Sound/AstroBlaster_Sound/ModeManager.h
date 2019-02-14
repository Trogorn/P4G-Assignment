// by Mark Featherstone (C) 2015 All Rights Reserved.

#ifndef MODEMANAGER_H
#define MODEMANAGER_H

// Application includes
#include <vector>
#include "Singleton.h"


// Forward declarations
class Mode;

//-------------------------------------------------------------------
// ModeManager
//-------------------------------------------------------------------
class ModeManager : public Singleton<ModeManager>
{
public:
	ModeManager();
	~ModeManager();

	// Update all currently active game modes
	void Update(float aTime);

	// Render all currently visible game modes
	void Render(float dTime);

	// Add a mode instance to the mode manager
	void AddMode(Mode* apMode);

	// Gets the mode instance with the given name
	Mode* GetMode(const char* apModeName);
	Mode *GetMode(int hash);

	void SwitchMode(const char *apModeName);

	void OnKey(char key);
	void Init();

private:
	//handy typedef for a Mode* array
	typedef std::vector<Mode*> Modes;
	Modes mModes;
	//are we trying to switch to a new mode
	Mode *mpNextMode;
	unsigned int mNextModeHash;
};

// Declare the static singleton accessor method
SINGLETON_GET(ModeManager);


#endif

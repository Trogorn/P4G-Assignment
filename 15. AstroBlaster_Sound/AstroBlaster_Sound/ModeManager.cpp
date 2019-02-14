//-------------------------------------------------------------------
// ModeManager.cpp
// Singleton class that keeps track of all game modes, allowing them
// to be updated and rendered as needed
//-------------------------------------------------------------------


#include "CRC.h"
#include "Error.h"
#include "UtilsDefines.h"

// Application includes
#include "Mode.h"
#include "ModeManager.h"
#include "Error.h"

#include "WindowUtils.h"

using namespace UDEF;


void ModeManager::OnKey(char key)
{
	for (unsigned int i = 0; i < mModes.size(); i++)
	{
		if (!mModes[i]->IsActive())
			continue;
		mModes[i]->OnKey(key);
	}
}

void ModeManager::Init()
{
	for (unsigned int i = 0; i < mModes.size(); i++)
		mModes[i]->Init();
}

ModeManager::ModeManager() : mpNextMode(0), mNextModeHash(UDEF::MAX_UINT)
{
}

//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
ModeManager::~ModeManager()
{
	for (unsigned int i = 0; i < mModes.size(); i++)
	{
		Mode* lpMode = mModes[i];
		delete lpMode;
	}
	mModes.clear();
}

void ModeManager::SwitchMode(const char *apModeName)
{
	mpNextMode = GetMode(apModeName);
}

//-------------------------------------------------------------------
// Update all currently active game modes
//-------------------------------------------------------------------
void ModeManager::Update(float aTime)
{
	// Call the update method of all currently active modes
	Mode *pActive=NULL;
	for (unsigned int i = 0; i < mModes.size(); i++)
	{
		if(!mModes[i]->IsActive())
			continue;
		pActive=mModes[i];
		pActive->Update(aTime);
		if(!mpNextMode)
			break;
		MPOD_ASSERT_MSG(pActive!=mpNextMode,"Switching to current mode");
		pActive->Exit();
		mpNextMode->Enter();
		pActive=mpNextMode;
		mpNextMode=nullptr;
		break;
	}
	if(!pActive)
	{
		MPOD_ASSERT_MSG(mpNextMode,"No active mode");
		pActive=mpNextMode;
		mpNextMode=nullptr;
		pActive->Enter();
	}
}

//-------------------------------------------------------------------
// Render all currently visible game modes
//-------------------------------------------------------------------
void ModeManager::Render(float dTime)
{
	// Call the render method of all currently visible modes
	for (unsigned int i = 0; i < mModes.size(); i++)
	{
		Mode* lpMode = mModes[i];
		if (lpMode->IsVisible())
			lpMode->Render(dTime);
	}
	if (GetIsPaused())
	{
	}
}

//-------------------------------------------------------------------
// Add a mode instance to the mode manager.  This method is called in
// Mode base class constructor, so should never need to be called
// by the user
//-------------------------------------------------------------------
void ModeManager::AddMode(Mode* apMode)
{
	mModes.push_back(apMode);
}

//-------------------------------------------------------------------
// Gets the mode instance with the given name
//-------------------------------------------------------------------
Mode* ModeManager::GetMode(const char* apModeName)
{
	unsigned int lHash = SCRC::GenerateCRC32(apModeName, strlen(apModeName) ); //IwHashString(apModeName);

	for (unsigned int i = 0; i < mModes.size(); i++)
	{
		Mode* lpMode = mModes[i];
		if (lpMode->HasHash(lHash))
			return lpMode;
	}

	MPOD_ASSERT_MSG(false, "Unable to find a registered Mode called '%s'", apModeName);
	return nullptr;
}

Mode * ModeManager::GetMode( int hash )
{
	for(Modes::iterator it=mModes.begin(); it!=mModes.end(); ++it)
	{
		if((*it)->GetHash()==hash)
			return *it;
	}
	MPOD_ASSERT(false);
	return nullptr;
}



// by Mark Featherstone (C) 2015 All Rights Reserved.

#ifndef MODE_H
#define MODE_H

#include <string>

/*
Modes are really finite state machines
Modes are things like: intro, playing the game, game over
The game mode is going to be the biggest, need to take
care it doesn't get too big. The class defines common
behaviour to all game modes.
*/
class Mode
{
public:
	Mode(const char* apName);
	virtual ~Mode()	{}

	// is the mode active and being rendered
	bool IsActive() const				{ return mActive; }
	bool IsVisible() const				{ return mVisible; }
	void SetVisible(bool visible) { mVisible=visible; }
	// modes are named, but the names are used to generate a unique ID
	bool HasHash(unsigned int aHash) const	{ return mNameHash == aHash; }
	unsigned int GetHash() const { return mNameHash; }
	bool HasName(const std::string &name);

	// Update method is called by ModeManager::Update to update the current
	// status of this mode if its active flag is set
	virtual	void Update(float aTime) {}

	// Render method is called by ModeManager::Render to display this mode,
	// if its visible flag is set
	virtual void Render(float dTime) {}

	//when switching mode let the current tidy up first
	virtual void Exit() { mActive=false; mVisible=false; }
	//when entering a new mode let it prepare itself
	virtual void Enter() { mActive=true; mVisible=true; }
	//go from window to full screen and some resources need resetting
	virtual void OnResetDevice() {};
	virtual void OnLostDevice() {};
	//done once at start, don't add anything slow!
	virtual void Init() {};
	//respond to key presses
	virtual void OnKey(char key) {}

private:
	// When true, this mode is active
	bool	mActive;

	// When true, this mode is visible
	bool	mVisible;

	// Hash value of the name string passed into the object constructor
	unsigned int	mNameHash;
};


#endif

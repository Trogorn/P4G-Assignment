// by Mark Featherstone (C) 2015 All Rights Reserved.

#include "CRC.h"

// Application includes
#include "Mode.h"
#include "ModeManager.h"


Mode::Mode(const char* apName) :
	mActive(false), mVisible(false)
{

	mNameHash = SCRC::GenerateCRC32(apName, strlen(apName));
	GetModeManager()->AddMode(this);
}

bool Mode::HasName(const std::string &name)
{
	unsigned int h=SCRC::GenerateCRC32(name.c_str(), (int)name.length());
	return h==mNameHash;
}
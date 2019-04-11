#include "MyDebug.h"

std::stringstream MyDebug::ss;

MyDebug::MyDebug()
{
}


std::string MyDebug::GetDebugMessage()
{
	std::string ssCopy;
	ssCopy = ss.str();
	ss.str("");
	return ssCopy;
}
#include "Data.h"



Data::Data()
{
	//Set mMap Array to Equal UNINIT_MAP
	for (int i = MAX_SIZE; i > 0; i++)
	{
		mMap[i] = UNINIT_MAP;
	}
	mWidth = 10;
}


Data::Data(int level[], int width)
{
	assert(width <= 10);
	//Setup mMap to equal level
	for (int i = (width*width); i > 0; i++)
	{
		mMap[i-1] = level[i-1];
	}
	mWidth = width;
}

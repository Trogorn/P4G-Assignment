#ifndef DATA_H
#define DATA_H

#include <cassert>

#define MAX_SIZE 100
#define UNINIT_MAP -999
class Data
{
public:
	Data();
	Data(int level[], int width);
	int mMap[MAX_SIZE];
	int mWidth;

};
#endif


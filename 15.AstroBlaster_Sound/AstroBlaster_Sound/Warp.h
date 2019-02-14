// by Mark Featherstone (C) 2015 All Rights Reserved.
#ifndef WARP_H
#define WARP_H

#include <vector>

class GameObject;

/*
When things enter the game the player can see them so 
cover them with a scifi warp-in effect
*/
class Warp
{
private:
	struct Data
	{
		typedef enum { FADE_IN, FADE_OUT } Mode;
		Data() : pObj(NULL), mTime(0), mInUse(false), mMode(FADE_IN) {}
		GameObject *pObj;	//an object warping in
		float mTime;		//time it
		Mode mMode;			//are we fading in or out?
		bool mInUse;		//is it currently warping
	};
	//create a handy typedef for an array of our warp data
	typedef std::vector<Data> Objects;
	//the actual data
	Objects mObjects;
public:
	
	Warp();
	~Warp();
	//an object wants to spawn into the world
	void AddObject(GameObject &obj);

	void Update(float dTime);
	void Render();
};

#endif
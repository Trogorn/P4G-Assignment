// by Mark Featherstone (C) 2015 All Rights Reserved.
// all game objects are owned by the manager

#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include <vector>
#include <unordered_map>

#include "Singleton.h"

class GameObject;

/*
We only ever want one game object manager so it's a singleton
All game objects are allocated at the start and then re-used
we don't want any memory allocation at run time
*/
class GameObjectManager : public Singleton<GameObjectManager>
{
public:
	GameObjectManager() {};
	//actually deletes all the game objects so it's vital
	//the game isn't trying to use any of them
	~GameObjectManager(); 
	//given one game object, create however many we need
	//this is called only at the start during loading
	void Add(GameObject *pObj, int number);
	//given an object type and an instance id, find the 
	//actual object and return it
	GameObject *GetObject(int type, int id)
	{
		Library::iterator it = mLibrary.find(type);
		MPOD_ASSERT_MSG(it!=mLibrary.end(),"Type not found");
		MPOD_ASSERT_MSG(id>=0 && id<(int)(*it).second.size(), "Bad object id");
		return (*it).second.at(id);
	}
	//if we are reusing objects then we need a way to search 
	//for a specific type of object that isn't currently being used
	GameObject *GetNextFree(int type);
	//check how many objects of a specific type are available
	int GetNum(int type)
	{
		Library::iterator it = mLibrary.find(type);
		if(it==mLibrary.end())
			return 0;
		return (int)(*it).second.size();
	}
	//the object isn't needed any more (for now) so it can
	//go back to being unused and ready for someone else
	void Release(GameObject *pObj);

private:
	//an array of game objects expressed as a convenient type
	typedef std::vector<GameObject*> GameObjects;
	/*
	remember game objects all have a type - a unique id
	so we can map each array of game objects of a specific type
	to that type i.e. give me all game objects of type "ASTEROID"
	where "ASTEROID" is 23476572
	*/
	typedef std::unordered_map<int, GameObjects> Library;
	//the actually data
	Library mLibrary;
};

SINGLETON_GET(GameObjectManager);

//a convenience macro to stop us repeating ourselves
#define GET_NEXT_FREE_OBJ(pObj, aClass)		\
	pObj=dynamic_cast<aClass*>( GetGameObjectManager()->GetNextFree(aClass::CalcType()));	





#endif
#include "GameObjectManager.h"
#include "GameObject.h"
#include "Error.h"


void GameObjectManager::Add(GameObject *pObj, int number)
{
	Library::iterator it = mLibrary.find(pObj->GetType());
	MPOD_ASSERT_MSG(it==mLibrary.end(), "Game object already exists");

	mLibrary[pObj->GetType()] = GameObjects();
	GameObjects *pObjs=&mLibrary[pObj->GetType()];
	pObjs->reserve(number);
	pObjs->push_back(pObj);
	for(int i=0;i<(number-1);++i)
		pObjs->push_back(pObj->Create());
}

void GameObjectManager::Release(GameObject *pObj)
{
	Library::iterator it = mLibrary.find(pObj->GetType());
	MPOD_ASSERT_MSG(it!=mLibrary.end(),"Type not found");
	pObj->SetInUse(false);
}


GameObjectManager::~GameObjectManager() 
{
	for(Library::iterator it=mLibrary.begin(); it!=mLibrary.end(); ++it)
	{
		for(GameObjects::iterator itObj=(*it).second.begin(); itObj!=(*it).second.end(); ++itObj)
		{
			GameObject *p = *itObj;
			delete p;
		}
	}
	mLibrary.clear();
}

GameObject *GameObjectManager::GetNextFree(int type)
{
	Library::iterator it = mLibrary.find(type);
	MPOD_ASSERT_MSG(it!=mLibrary.end(),"Type not found");
	for(GameObjects::iterator itObj=(*it).second.begin(); itObj!=(*it).second.end(); ++itObj)
	{
		if((*itObj)->GetInUse())
			continue;
		(*itObj)->SetInUse(true);
		(*itObj)->Init();
		return (*itObj);
	}
	return NULL;
}




// by Mark Featherstone (C) 2015 All Rights Reserved.
//the main game mode

#ifndef MODEGAME_H
#define MODEGAME_H

#include <vector>


// Application includes
#include "Mode.h"
#include "Singleton.h"
#include "UtilsDefines.h"
#include "Starfield.h"
#include "Camera.h"
#include "Warp.h"
#include "Hud.h"


// Forward declarations
class GameObject;
class Ship;
class Asteroid;
enum class AsteroidType;

// Definitions
#define QUIET_VOLUME 0.25f
#define NORMAL_VOLUME 0.7f
#define LOUD_VOLUME 1.f


class ModeGame : public Mode, public Singleton<ModeGame>
{
public:
	typedef std::vector<GameObject*> GameObjects;	//handy typedef for an array of game objects
	static const float HALF_WORLD;		//the world is square, this is half its width

	ModeGame();
	virtual ~ModeGame();

	// Virtual methods from Mode
	void Update(float aTime) override;
	void Render(float dTime) override;
	void Enter() override;
	void Exit() override;
	void Init() override;
	void OnKey(char key) override;

	//the game has its own array of active objects in the world
	void AddGameObject(GameObject *pObj) {
		mGameObjects.push_back(pObj);
	}
	//when a ship or asteroid dies it has to be removed from the game
	void RemoveGameObject(GameObject *pObj);

	//main game camera
	Camera &GetCamera() { return mCamera; }

	//a new asteroid is spawning in
	Asteroid *AddAsteroid(AsteroidType type, const DirectX::SimpleMath::Vector2 &desiredPos, const DirectX::SimpleMath::Vector2 *pMoveFrom, GameObject *pSpawner, bool spawnAnywhere = false);

	//explosions create an expanding force that pushes and damages other objects
	void ApplyForce(const DirectX::SimpleMath::Vector2 &centre, float radius, float magnitude, GameObject *pIgnore);

	//blow stuff up and the score rises
	void AddScore( int score) { mScore+=score; }

	//accessors
	int GetLives() const { return mLives; }
	void SetLives( int lives) { mLives=lives; }
	int GetScore() const { return mScore; }
	const GameObjects &GetGameObjects() { return mGameObjects; }

	//there's only one player's ship
	Ship &GetShip() { return *mpShip; }
	
	//test an object or just a circle to see if it is colliding with anything
	bool IsInCollision(const GameObject &obj, const GameObject *pIgnore);
	bool IsInCollision(const DirectX::SimpleMath::Vector2 &centre, float radius, const GameObject *pIgnore, const GameObject *pIgnore2);

	//game gets harder as it levels up
	int GetLevel() const { return mLevel; }

	//as objects enter we use a warp effect to mask it
	Warp &GetWarp() { return mWarp; }


private:

	// The GameObjects that make up the game world
	GameObjects mGameObjects;

	//we cannot just remove a game object from the game, others might be referencing it
	//so we make a list of things wanting out and then after everyone has updated and
	//its safe, we return them to their manager for re-use
	GameObjects mWaitingForRemoval;	
	Starfield mStars;
	Camera mCamera;	//main game camera
	
	Ship *mpShip;		//oen and only player's ship

	Hud mHud;			//radar, score, lives, etc

	int mLives;		//3 lives for the player

	int mScore;		//player's score

	unsigned int mMusicHdl;	//the background music

	int mLevel;		//destroy all asteroids, start a new level

	Warp mWarp;

	bool mMakeHoming;		//if only little asteroids left, make them home in on the player

	//delete the entry for this object from the games array, tell the 
	//manager we don't need it anymore
	void ReleaseObject(GameObject &obj);	

	void NextLevel();		//setup for a new level

	//go through each object and let it update
	void UpdateObjects( float aTime );

	//test each object for collision with ever other object
	void CheckCollision();

	void AddToWorld(GameObject *pObj, const DirectX::SimpleMath::Vector2 &desiredPos, const DirectX::SimpleMath::Vector2 *pMoveFrom, GameObject *pSpawner, bool spawnAnywhere);
};

// Declare the static singleton access method
SINGLETON_GET(ModeGame);


#endif
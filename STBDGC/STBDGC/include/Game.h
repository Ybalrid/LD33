#ifndef GAME
#define GAME
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <Ogre.h>
#include "InputManager.hpp"
#include "Monster.hpp"

#include "GamePhysics.hpp"
class Game
{
public:
	Game();
	~Game();
	void run();
private:

	void turnOffLight();
	void turnOnLight();

	void update();
	Ogre::Root* root;
	Ogre::RenderWindow* window;
	Ogre::SceneManager* smgr;
	Ogre::SceneNode* camera;
	Ogre::Camera* cameraObj;
	Ogre::Viewport* viewport;
	Ogre::ResourceGroupManager* resourceManager;
	bool gameRunning;

	//Custom game classes:
	GameInputManager* gameInputManager;
	float mouseSpeed, walkSpeed;
	int mouseX, mouseY;

	//The player IS THE MONSTER!! (that's the theme of the jam...)
	Monster* monster;

	void refreshTimer();
	unsigned long deltaTime;
	unsigned long lastTimeSinceStartup;
	unsigned long currentTimeSinceStartup;

	Ogre::SceneNode* staticGeometry;


	//Scene content
	Ogre::Light* roomLight;
	Ogre::ColourValue off, on;

	GamePhysics* gamePhysics;
	void createStaticRigidBody(Ogre::SceneNode* node, Ogre::Entity* entity);
	btRigidBody* monsterCapsule;
};

#endif
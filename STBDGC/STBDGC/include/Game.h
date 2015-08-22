#ifndef GAME
#define GAME

#include <Ogre.h>
#include "InputManager.hpp"
#include "Monster.hpp"
class Game
{
public:
	Game();
	~Game();
	void run();
private:
	void update();
	Ogre::Root* root;
	Ogre::RenderWindow* window;
	Ogre::SceneManager* smgr;
	Ogre::Camera* camera;
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

};

#endif
#ifndef GAME
#define GAME

#include <Ogre.h>

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

};

#endif
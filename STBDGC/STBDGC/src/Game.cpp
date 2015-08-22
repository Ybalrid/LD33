#include "Game.h"

Game::Game() :
	root(nullptr),
	window(nullptr),
	gameRunning(false)
{
	Ogre::String plugin;
#ifdef DEBUG
	plugin = "plugin_d.cfg";
#else
	plugin = "plugin.cfg";
#endif
	root = new Ogre::Root(plugin, "ogre.cfg");
	if(!root->restoreConfig())
		if(!root->showConfigDialog())
			exit(-1);

	window = root->initialise(true, "Scare them... But don't get cought!");


	smgr=root->createSceneManager("OctreeSceneManager");
	camera=smgr->createCamera("Main_Cam");
	viewport=window->addViewport(camera);
	viewport->setBackgroundColour(Ogre::ColourValue::Blue);
	gameRunning=true;

	resourceManager = Ogre::ResourceGroupManager::getSingletonPtr();
	resourceManager->addResourceLocation("./res", "FileSystem", "GAME", true);
	resourceManager->initialiseResourceGroup("GAME");

}

Game::~Game()
{
	delete root;
}

void Game::run()
{
	
	while(gameRunning)
		update();
}

void Game::update()
{
	Ogre::WindowEventUtilities::messagePump();
	root->renderOneFrame();
	if(window->isClosed())gameRunning=false;
}

#include "pch.hpp"
#include "Game.h"

Game::Game() :
	root(nullptr),
	window(nullptr),
	gameRunning(false),
	mouseSpeed(10),
	walkSpeed(3)
{
	monster = new Monster;
	//Select the correct set of plugin to load
	Ogre::String plugin;
#ifdef DEBUG
	plugin = "plugin_d.cfg";
#else
	plugin = "plugin.cfg";
#endif
	root = new Ogre::Root(plugin, "ogre.cfg");

	//Get the configuration
	if(!root->restoreConfig())
		if(!root->showConfigDialog())
			exit(-1);

	//Create the window folowing config
	window = root->initialise(true, "Scare them... But don't get cought!");
	gameInputManager = new GameInputManager(window);


	smgr=root->createSceneManager("OctreeSceneManager");
	smgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	camera=smgr->createCamera("Main_Cam");
	viewport=window->addViewport(camera);
	viewport->setBackgroundColour(Ogre::ColourValue::Blue);
	gameRunning=true;


	//Tmp------------- testing
	monster->setPlanarCoordinates(Ogre::Vector2(1.75, -1));

	camera->setPosition(monster->getPlanarCoodinates().x, monster->getHeight(), monster->getPlanarCoodinates().y);
	camera->lookAt(0,monster->getHeight(),-1);
	camera->setNearClipDistance(0.0001);
	camera->setFarClipDistance(10000);
	resourceManager = Ogre::ResourceGroupManager::getSingletonPtr();
	resourceManager->addResourceLocation("./res/graphics", "FileSystem", "GAME", true);
	resourceManager->initialiseResourceGroup("GAME");

	staticGeometry = smgr->getRootSceneNode()->createChildSceneNode();
	staticGeometry->attachObject(smgr->createEntity("floor_walls.mesh"));
	staticGeometry->attachObject(smgr->createEntity("bed_struct.mesh"));
	staticGeometry->attachObject(smgr->createEntity("bookshelf.mesh"));
	staticGeometry->attachObject(smgr->createEntity("mattress.mesh"));
	staticGeometry->attachObject(smgr->createEntity("nightstand.mesh"));
	staticGeometry->attachObject(smgr->createEntity("nightstand.drawer.mesh"));
	staticGeometry->attachObject(smgr->createEntity("toybox.mesh"));


	roomLight =  smgr->createLight();
	roomLight->setPosition(0,1,0);
	roomLight->setDiffuseColour(.5f,.5f,.5f);
	
	lastTimeSinceStartup = root->getTimer()->getMilliseconds();
}

Game::~Game()
{
	delete monster;
	delete gameInputManager;
	delete root;
}

void Game::run()
{
	
	while(gameRunning)
		update();
}

float sec(unsigned long milli)
{
	return float(milli)/1000.0f;
}
void Game::update()
{
	refreshTimer();
	Ogre::WindowEventUtilities::messagePump();

	gameInputManager->capture();

	mouseX = gameInputManager->getMouse()->getMouseState().X.rel;
	mouseY = gameInputManager->getMouse()->getMouseState().Y.rel;

	camera->yaw(-Ogre::Degree(mouseX*mouseSpeed*sec(deltaTime)));
	camera->pitch(-Ogre::Degree(mouseY*mouseSpeed*sec(deltaTime)));

	OIS::Keyboard* keyboard = gameInputManager->getKeyboard();
	Ogre::Vector3 translate;
	bool displacement(false);
	if(keyboard->isKeyDown(OIS::KC_W) || keyboard->isKeyDown(OIS::KC_UP))
	{
		displacement = true;
		translate += camera->getOrientation() * Ogre::Vector3(0,0,-1);
	}
	if(keyboard->isKeyDown(OIS::KC_A) || keyboard->isKeyDown(OIS::KC_LEFT))
	{		
		displacement = true;
		translate += camera->getOrientation() * Ogre::Vector3(-1,0,0);
	}
	if(keyboard->isKeyDown(OIS::KC_S) || keyboard->isKeyDown(OIS::KC_DOWN))
	{		
		displacement = true;
		translate += camera->getOrientation() * Ogre::Vector3(0,0,1);
	}
	if(keyboard->isKeyDown(OIS::KC_D) || keyboard->isKeyDown(OIS::KC_RIGHT))
	{		
		displacement = true;
		translate += camera->getOrientation() * Ogre::Vector3(1,0,0);
	}
	
	if(displacement)
		translate.normalise();
	translate *= sec(deltaTime)*walkSpeed;

	monster->setPlanarCoordinates(monster->getPlanarCoodinates() + Ogre::Vector2(translate.x, translate.z));
	camera->setPosition(monster->getPlanarCoodinates().x, monster->getHeight(), monster->getPlanarCoodinates().y);
	
	root->renderOneFrame();

	//Stop the gameloop if the window is closed (quit game)
	if(window->isClosed())gameRunning=false;
}

void Game::refreshTimer()
{
	currentTimeSinceStartup = root->getTimer()->getMilliseconds();
	deltaTime =  currentTimeSinceStartup - lastTimeSinceStartup;
	lastTimeSinceStartup = currentTimeSinceStartup;
}

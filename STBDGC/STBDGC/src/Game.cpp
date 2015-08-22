#include "pch.hpp"
#include "Game.h"

void sleep(int millisec)
{
#ifdef _WIN32
	Sleep(millisec);
#else
	usleep(millec*1000);
#endif
}

Game::Game() :
	root(nullptr),
	window(nullptr),
	gameRunning(false),
	mouseSpeed(10),
	walkSpeed(3),
	off(.1f,.2f,.5f),
	on(Ogre::ColourValue::White)
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
	gamePhysics = new GamePhysics(smgr->getRootSceneNode());

	cameraObj=smgr->createCamera("Main_Cam");
	cameraObj->setAutoAspectRatio(true);
	viewport=window->addViewport(cameraObj);
	camera = smgr->getRootSceneNode()->createChildSceneNode();
	//camera->attachObject(cameraObj);
	viewport->setBackgroundColour(Ogre::ColourValue::Blue);
	gameRunning=true;

	resourceManager = Ogre::ResourceGroupManager::getSingletonPtr();
	resourceManager->addResourceLocation("./res/graphics", "FileSystem", "GAME", true);
	resourceManager->initialiseResourceGroup("GAME");


	staticGeometry = smgr->getRootSceneNode()->createChildSceneNode();

	Ogre::SceneNode* tmpNode; Ogre::Entity* tmpEntity;
	(tmpNode = staticGeometry->createChildSceneNode())->attachObject(tmpEntity = smgr->createEntity("floor_walls.mesh"));
	createStaticRigidBody(tmpNode,tmpEntity);
	(tmpNode = staticGeometry->createChildSceneNode())->attachObject(tmpEntity = smgr->createEntity("bed_struct.mesh"));
	createStaticRigidBody(tmpNode,tmpEntity);
	(tmpNode = staticGeometry->createChildSceneNode())->attachObject(tmpEntity = smgr->createEntity("bookshelf.mesh"));
	createStaticRigidBody(tmpNode,tmpEntity);
	(tmpNode = staticGeometry->createChildSceneNode())->attachObject(tmpEntity = smgr->createEntity("mattress.mesh"));
	createStaticRigidBody(tmpNode,tmpEntity);
	(tmpNode = staticGeometry->createChildSceneNode())->attachObject(tmpEntity = smgr->createEntity("nightstand.mesh"));
	createStaticRigidBody(tmpNode,tmpEntity);
	(tmpNode = staticGeometry->createChildSceneNode())->attachObject(tmpEntity = smgr->createEntity("nightstand.drawer.mesh"));
	createStaticRigidBody(tmpNode,tmpEntity);
	(tmpNode = staticGeometry->createChildSceneNode())->attachObject(tmpEntity = smgr->createEntity("toybox.mesh"));
	createStaticRigidBody(tmpNode,tmpEntity);


	monster->setPlanarCoordinates(Ogre::Vector2(1.75, -1));

	camera->setPosition(monster->getPlanarCoodinates().x, monster->getHeight()+.1f, monster->getPlanarCoodinates().y);
	cameraObj->lookAt(0,0,0);
	cameraObj->setNearClipDistance(0.0001);
	cameraObj->setFarClipDistance(10000);

	roomLight =  smgr->createLight();
	roomLight->setPosition(0,1,0);
	roomLight->setDiffuseColour(on);
	
	lastTimeSinceStartup = root->getTimer()->getMilliseconds();

	btCollisionShape* monsterCapsuleShape = new btCapsuleShape(0.125f, monster->getHeight() - 2*0.125f);
	BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(camera);
	btVector3 inertia;
	monsterCapsuleShape->calculateLocalInertia(100,inertia);
	monsterCapsule = new btRigidBody(100,state,monsterCapsuleShape,inertia);
	gamePhysics->getWorld()->addRigidBody(monsterCapsule);

}

void Game::createStaticRigidBody(Ogre::SceneNode* node, Ogre::Entity* entity)
{
	BtOgre::StaticMeshToShapeConverter converter(entity);
	btTriangleMeshShape* shape = converter.createTrimesh();
	btVector3 inertia(0,0,0);
	BtOgre::RigidBodyState* state = new BtOgre::RigidBodyState(node);
	btRigidBody* body = new btRigidBody(0, state, shape, inertia);
	gamePhysics->getWorld()->addRigidBody(body);
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

	monsterCapsule->activate();

	Ogre::WindowEventUtilities::messagePump();

	gameInputManager->capture();

	mouseX = gameInputManager->getMouse()->getMouseState().X.rel;
	mouseY = gameInputManager->getMouse()->getMouseState().Y.rel;

	cameraObj->yaw(-Ogre::Degree(mouseX*mouseSpeed*sec(deltaTime)));
	cameraObj->pitch(-Ogre::Degree(mouseY*mouseSpeed*sec(deltaTime)));

	OIS::Keyboard* keyboard = gameInputManager->getKeyboard();
	Ogre::Vector3 translate(Ogre::Vector3::ZERO);
	bool displacement(false);

	if(keyboard->isKeyDown(OIS::KC_W))
	{
		displacement = true;
		translate.z -=1;
	}
		
	if(keyboard->isKeyDown(OIS::KC_A))
	{
		displacement = true;
		translate.x -=1;

	}	
	
	if(keyboard->isKeyDown(OIS::KC_S))
	{
		displacement = true;
		translate.z +=1;
	}	
	
	if(keyboard->isKeyDown(OIS::KC_D))
	{
		displacement = true;
		translate.x +=1;
	}

	translate = cameraObj->getOrientation()*translate;
	translate.normalise();
	btTransform transform = monsterCapsule->getCenterOfMassTransform();
	Ogre::Quaternion fixedOrient(Ogre::Quaternion::IDENTITY);
	transform.setRotation(btQuaternion(fixedOrient.x, fixedOrient.y, fixedOrient.z, fixedOrient.w));
	monsterCapsule->setCenterOfMassTransform(transform);

	btVector3 currentVelocity = monsterCapsule->getLinearVelocity();
	if(!displacement)
	{
		monsterCapsule->setLinearVelocity(btVector3(0,currentVelocity.y(),0));
	}
	else
	{
		translate*=walkSpeed;
		translate = cameraObj->getOrientation().IDENTITY * translate;
		monsterCapsule->setLinearVelocity(btVector3(translate.x,currentVelocity.y(),translate.z));
	}

	//keep standing
	
	gamePhysics->step(deltaTime);
	cameraObj->setPosition(camera->getPosition() + Ogre::Vector3(0,monster->getHeight()/2,0));

	root->renderOneFrame();

	//If vsync isn't enabled
	if(!window->isVSyncEnabled())
	if(deltaTime < (1.0f/240.0f)*1000)
		//limit framerate to prevent timing errors and to offload CPU
		sleep((1.0f/240.0f)*1000 - deltaTime);

	//Stop the gameloop if the window is closed (quit game)
	if(window->isClosed())gameRunning=false;
	if(keyboard->isKeyDown(OIS::KC_ESCAPE))gameRunning = false;
	if(keyboard->isKeyDown(OIS::KC_F1))turnOffLight();
	if(keyboard->isKeyDown(OIS::KC_F2))turnOnLight();


}

void Game::refreshTimer()
{
	currentTimeSinceStartup = root->getTimer()->getMilliseconds();
	deltaTime =  currentTimeSinceStartup - lastTimeSinceStartup;
	lastTimeSinceStartup = currentTimeSinceStartup;
}

void Game::turnOffLight()
{
	roomLight->setDiffuseColour(off);
}

void Game::turnOnLight()
{
	roomLight->setDiffuseColour(on);
}

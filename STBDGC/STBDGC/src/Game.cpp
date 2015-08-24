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
using namespace Ogre;
Game::Game() :
	root(nullptr),
	window(nullptr),
	gameRunning(false),
	mouseSpeed(10),
	walkSpeed(3),
	off(.1f,.2f,.5f),
	on(Ogre::ColourValue::White),
	playing(nullptr),
	debounceE(false),
	totalCry(0),
	gameover(false)
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

	audioEngine = new AudioEngine;
	audioEngine->playBGM("./res/bgm/LD33_music.Wav");


	//Create the window folowing config
	window = root->initialise(true, "Scare them... But don't get caught!");
	gameInputManager = new GameInputManager(window);


	smgr=root->createSceneManager("OctreeSceneManager", "MAIN_SMGR");
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


	monster->setPlanarCoordinates(Ogre::Vector2(1.75, 0));

	camera->setPosition(monster->getPlanarCoodinates().x, monster->getHeight()+.1f, monster->getPlanarCoodinates().y);
	cameraObj->setNearClipDistance(0.01);
	cameraObj->setFarClipDistance(10000);

	roomLight =  smgr->createLight();
	roomLight->setPosition(0,1,0);
	roomLight->setDiffuseColour(on);

	corridorLight = smgr->createLight();
	corridorLight->setPosition(0,1,3);
	corridorLight->setDiffuseColour(on);

	lastTimeSinceStartup = root->getTimer()->getMilliseconds();

	btCollisionShape* monsterCapsuleShape = new btCapsuleShape(0.125f, monster->getHeight() - 2*0.125f);
	BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(camera);
	btVector3 inertia;
	monsterCapsuleShape->calculateLocalInertia(100,inertia);
	monsterCapsule = new btRigidBody(100,state,monsterCapsuleShape,inertia);
	gamePhysics->getWorld()->addRigidBody(monsterCapsule);

	Ogre::SceneNode* boy = smgr->getRootSceneNode()->createChildSceneNode();
	littleBoy = new LittleBoy(boy, smgr);
	boy->pitch(Ogre::Degree(-90));
	boy->setPosition(-1.57, .3385f,- .5252);
	littleBoy->play(LittleBoy::idleSleep);

	cameraObj->yaw(Ogre::Degree(80));
	cameraObj->pitch(Ogre::Degree(-10));

	currentState = TITLE;
	keyboard = gameInputManager->getKeyboard();

	monster->footstepBuffer = audioEngine->loadSndFile("./res/sfx/footstep.wav");
	alGenSources(1,&monster->footstepSource);
	alSourcei(monster->footstepSource, AL_BUFFER, monster->footstepBuffer);
	alSourcei(monster->footstepSource, AL_LOOPING, AL_TRUE);
	alSourcef(monster->footstepSource, AL_GAIN, 1);
	//alSourcePlay(monster->footstepSource);

	littleBoy->cryBuffer = audioEngine->loadSndFile("./res/sfx/screaming.wav");
	alGenSources(1,&littleBoy->crySource);
	alSourcei(littleBoy->crySource, AL_BUFFER, littleBoy->cryBuffer);
	alSourcei(littleBoy->crySource, AL_LOOPING, AL_TRUE);
	alSourcef(littleBoy->crySource, AL_GAIN,0);
	alSourcePlay(littleBoy->crySource);

	roomLit = true;
	corridorLit = true;
	currentEvent = NONE;


	momNode = smgr->getRootSceneNode()->createChildSceneNode();
	momNode->setPosition(-.5f,0,-1);
	mom = new Mom(momNode, smgr);

	smgr->addRenderQueueListener(new Ogre::OverlaySystem);
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay* overlay = overlayManager.create("HUD");

	if(!Ogre::FontManager::getSingletonPtr())
		new Ogre::FontManager;

	Ogre::FontPtr font = Ogre::FontManager::getSingleton().create("DejaVuSans",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	font->setType(Ogre::FontType::FT_TRUETYPE);
	font->setSource("DejaVuSans.ttf");
	font->setTrueTypeResolution(96);
	font->setTrueTypeSize(24);


	// Create a panel
	Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "PanelName" ) );
	//panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setPosition(0, 0);
	panel->setDimensions(1, 1);
	//panel->setMaterialName( "BaseWhite" );
	// Add the panel to the overlay
	textArea = static_cast<TextAreaOverlayElement*>(
		overlayManager.createOverlayElement("TextArea", "TextAreaName"));
	textArea->setMetricsMode(Ogre::GMM_RELATIVE);
	textArea->setPosition(0, 0);
	textArea->setDimensions(1,1);
	textArea->setCaption("Hello, World!");
	textArea->setCharHeight(1.0f/30.0f);
	textArea->setFontName("DejaVuSans");
	//
	textArea->setColourBottom(ColourValue::White);
	textArea->setColourTop(ColourValue::White);



	panel->addChild(textArea);


	titleScreen = static_cast<TextAreaOverlayElement*>(
		overlayManager.createOverlayElement("TextArea", "TitleScreen"));
	titleScreen->setMetricsMode(Ogre::GMM_RELATIVE);
	titleScreen->setPosition(0.1,0.5-(1.0f/15.0f));
	titleScreen->setDimensions(0.5,0.5);
	titleScreen->setCaption("Press ENTER to Start...");
	titleScreen->setCharHeight(1.0f/15.0f);
	titleScreen->setFontName("DejaVuSans");
	titleScreen->setColourBottom(ColourValue::White);
	titleScreen->setColourTop(ColourValue::White);
	//titleScreen->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Center);
	panel->addChild(titleScreen);
	titleScreen->show();

	overlay->add2D( panel );

	overlay->show();
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
	/*std::stringstream ss;
	ss << "Time : " << currentTimeSinceStartup / 1000 << std::endl;
	ss << "FPS : " << window->getAverageFPS() << std::endl;
	ss << "LudumDare #33";
	if(textArea) textArea->setCaption(ss.str());*/
	textArea->setCaption(
		"You are the monster that lives in the closet...\nYou scare this little boy for a living... It's finaly bedtime...\nNow, you can go. But beware, parents are at home, You'll disapear if they see you...\n\nA game by Ybalrid for the Ludum Dare #33"

		);
	titleScreen->setCaption("Scare them... But don't get caught!\nPress ENTER to Start...");

	Ogre::WindowEventUtilities::messagePump();
	gameInputManager->capture();

	littleBoy->addTime(sec(deltaTime));
	monsterCapsule->activate();
	//Stop the gameloop if the window is closed (quit game)
	if(window->isClosed())gameRunning=false;	
	if(keyboard->isKeyDown(OIS::KC_ESCAPE))gameRunning=false;
	Ogre::Vector3 translate(Ogre::Vector3::ZERO);
	bool displacement(false);

	if(keyboard->isKeyDown(OIS::KC_RETURN)) 
	{
		currentState = GAME_LOOP;
		currentEvent = GAME_START;
		gameStatTime = currentTimeSinceStartup;
		titleScreen->hide();
	}

	if(currentState == GAME_LOOP && !gameover)
	{
		gameplay();
		mom->addTime(sec(deltaTime));
		std::stringstream ss;
		ss << "Monster pos " << monster->getPlanarCoodinates() << std::endl;
		ss << "Mom pos " << momNode->getPosition() << std::endl;
		ss << "isMomInBedroom() returns " << isMomInBedroom() << std::endl;
		ss << "littleBoy::isCrying() returns " << littleBoy->isCrying() << std::endl;
		ss << "isMonsterInBedroom() returns " << isMonsterInBedroom() << std::endl;
		ss << "isMonsterHidden() returns " << isMonsterHidden() << std::endl;
		//textArea->setCaption(ss.str());

		if(momNode->getPosition().z > 3.5) mom->walk = false;
		if( momNode->getPosition().z < -1) mom->walk = false;

		timeRelativeEvent();
		mouseX = gameInputManager->getMouse()->getMouseState().X.rel;
		mouseY = gameInputManager->getMouse()->getMouseState().Y.rel;

		cameraObj->yaw(-Ogre::Degree(mouseX*mouseSpeed*sec(deltaTime)));
		cameraObj->pitch(-Ogre::Degree(mouseY*mouseSpeed*sec(deltaTime)));
		//Clamp pitch angle to avoid strange stuff...
		if(cameraObj->getOrientation().getPitch() >= Ogre::Degree(90) || cameraObj->getOrientation().getPitch() <= Ogre::Degree(-90) )
			cameraObj->pitch (+Ogre::Degree(mouseY*mouseSpeed*sec(deltaTime)));

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
		alSourceStop(monster->footstepSource);
	}
	else
	{
		translate*=walkSpeed;
		translate = cameraObj->getOrientation().IDENTITY * translate;
		monsterCapsule->setLinearVelocity(btVector3(translate.x,currentVelocity.y(),translate.z));
		if(!monster->wasWalking)		
			alSourcePlay(monster->footstepSource);
	}

	monster->wasWalking = displacement;


	gamePhysics->step(deltaTime);
	cameraObj->setPosition(camera->getPosition() + Ogre::Vector3(0,monster->getHeight()/2,0));
	monster->setPlanarCoordinates(cameraObj->getPosition().x, cameraObj->getPosition().z);
	if(gameover) gameplay();
	root->renderOneFrame();

	//If vsync isn't enabled
	if(!window->isVSyncEnabled())
		if(deltaTime < (1.0f/240.0f)*1000)
			//limit framerate to prevent timing errors and to offload CPU
				sleep((1.0f/240.0f)*1000 - deltaTime);

	//debug
	float x = monster->getPlanarCoodinates().x;
	float y = monster->getPlanarCoodinates().x;

	if(x > -0.9 && x < 0.8)
		if(y > -1.77f && y < 0.8)
	if(debounceE && !keyboard->isKeyDown(OIS::KC_E))
	{
		littleBoy->play(LittleBoy::surpriseJump);
		littleBoy->changeIdleAnim(LittleBoy::idleCry);
		cryTimer = currentTimeSinceStartup;
		currentEvent = BOY_CRY;
	}

	if(keyboard->isKeyDown(OIS::KC_E))debounceE=true;
	if(!keyboard->isKeyDown(OIS::KC_E))debounceE=false;

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
	roomLit = false;
}

void Game::turnOnLight()
{
	roomLight->setDiffuseColour(on);
	roomLit = true;
}


void Game::turnOffCorridor()
{
	corridorLight->setDiffuseColour(off);
	corridorLit = false;
}


void Game::timeRelativeEvent()
{
	if(currentState = GAME_LOOP)
	{
		switch(currentEvent)
		{
		default:
		case NONE:
			return;
		case GAME_START:
			if(currentTimeSinceStartup-gameStatTime > 500)
				if(roomLit)
					turnOffLight();
			if(currentTimeSinceStartup-gameStatTime > 1200)
				if(corridorLit)
				{
					turnOffCorridor();
					currentEvent = NONE;
				}
				break;
		case BOY_CRY:
			if(currentTimeSinceStartup-cryTimer > 10000)
			{
				littleBoy->returnToSleep();
				currentEvent = NONE;
			}
			break;
		}

	}
}

bool Game::isMomInBedroom()
{
	if(momNode->getPosition().z > 2.1)
		return false;
	return true;
}


bool Game::isMonsterInBedroom()
{
	Ogre::Vector2 pos = monster->getPlanarCoodinates();
	if(pos.x < 1.1 && pos.y < 2.1)
		return true;
	return false;
}

bool Game::isMonsterHidden()
{
	Ogre::Vector2 pos = monster->getPlanarCoodinates();
	if(pos.x > 1.1 && pos.y > 0)
		return true;
	return false;

}

void Game::gameplay()
{

	if(lastMomWasInBedroom && !isMomInBedroom())
	{
		mom->walkspeed+=.1;
	}
	if(isMomInBedroom()) textArea->setCaption("Wait here untl the mother is not in the bedroom");
	

	if(littleBoy->isCrying() && cryTimer > 3000)
	{
		textArea->setCaption("Mom's comming!");
		mom->goToBedroom();
	}

	if(!littleBoy->isCrying() && isMomInBedroom())
	{
		textArea->setCaption("Beware... she's still here...\nWait here until the mother is not in the bedroom...");
		mom->goToCorridor();
	}

	if(!isMomInBedroom() && !isMomInBedroom())
	{
		textArea->setCaption("It looks like it's safe to go...");
			float x = monster->getPlanarCoodinates().x;
	float y = monster->getPlanarCoodinates().x;
	
		if(x > -0.9 && x < 0.8)
			if(y > -1.77f && y < 0.8)
		textArea->setCaption("Press E to scare the boy!");
	}

	if(littleBoy->isCrying())
	{
		textArea->setCaption("The boy is crying!!");
		if(!gameover)totalCry+= deltaTime;
	}

	if(isMomInBedroom() && !isMonsterHidden() && !mom->notGoingToBedroom())
	{

		std::stringstream ss; ss << "GameOver! stay hidden until the mother is not here!.\n The boy cryed for : " << sec(totalCry);
		textArea->setCaption(ss.str());
		mom->walk = false;
		gameover = true;
	}

	lastMomWasInBedroom = isMomInBedroom();
}
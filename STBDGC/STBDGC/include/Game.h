#ifndef GAME
#define GAME
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <Ogre.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#include <Overlay/OgreOverlayContainer.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreTextAreaOverlayElement.h>
#include <Overlay/OgreFontManager.h>
#include "InputManager.hpp"
#include "Monster.hpp"
#include "LittleBoy.hpp"
#include "Mom.h"
#include "GamePhysics.hpp"
#include "AudioEngine.hpp"

#include <vector>

class Game
{
public:
	Game();
	~Game();
	void run();
private:
	enum state {TITLE, GAME_LOOP, END };
	enum event {NONE, GAME_START, BOY_CRY};
	void turnOffLight();
	void turnOnLight();
	void turnOffCorridor();
	void turnOnCorridor();

	void update();
	Ogre::Root* root;
	Ogre::RenderWindow* window;
	Ogre::SceneManager* smgr;
	Ogre::SceneNode* camera;
	Ogre::Camera* cameraObj;
	Ogre::Viewport* viewport;
	Ogre::ResourceGroupManager* resourceManager;
	Ogre::TextAreaOverlayElement* textArea;
	Ogre::TextAreaOverlayElement* titleScreen;
	//Ogre::TextAreaOverlayElement* textArea;
	bool gameRunning;

	//Custom game classes:
	GameInputManager* gameInputManager;
	OIS::Keyboard* keyboard;
	float mouseSpeed, walkSpeed;
	int mouseX, mouseY;

	//The player IS THE MONSTER!! (that's the theme of the jam...)
	Monster* monster;

	void refreshTimer();
	unsigned long deltaTime;
	unsigned long lastTimeSinceStartup;
	unsigned long currentTimeSinceStartup;
	unsigned long gameStatTime;
	unsigned long cryTimer;
	unsigned long totalCry;


	Ogre::SceneNode* staticGeometry;


	//Scene content
	Ogre::Light* roomLight, * corridorLight;
	Ogre::ColourValue off, on;
	bool corridorLit, roomLit;

	GamePhysics* gamePhysics;
	void createStaticRigidBody(Ogre::SceneNode* node, Ogre::Entity* entity);
	btRigidBody* monsterCapsule;
	LittleBoy* littleBoy;
	Mom* mom;
	Ogre::SceneNode* momNode;
	
	std::vector<Ogre::AnimationState*>* playing;
	void addTimeToAnimationState(std::vector<Ogre::AnimationState*>* animationStatesToPlay);
	bool debounceE;

	AudioEngine* audioEngine;

	state currentState;
	event currentEvent;

	void timeRelativeEvent();

	bool isMomInBedroom();
	bool isMonsterInBedroom();
	bool isMonsterHidden();

	void gameplay();
	bool lastMomWasInBedroom;
	bool gameover;
};

#endif
#include "pch.hpp"
#include "InputManager.hpp"

GameInputManager::GameInputManager(Ogre::RenderWindow* renderWindow):
	keyboard(nullptr),
	mouse(nullptr),
	inputManager(nullptr)
{
	//Get the window handle
	size_t windowHandle;
	renderWindow->getCustomAttribute("WINDOW",&windowHandle);
	std::stringstream whndstr; whndstr << windowHandle;
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), whndstr.str()));

	//Create the OIS Input manager
	inputManager = OIS::InputManager::createInputSystem(pl);
	keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
	mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
}


GameInputManager::~GameInputManager()
{
	//input system cleanup
	inputManager->destroyInputObject(keyboard);
	inputManager->destroyInputObject(mouse);
	OIS::InputManager::destroyInputSystem(inputManager);
}

OIS::Keyboard* GameInputManager::getKeyboard()
{
	return keyboard;
}

OIS::Mouse* GameInputManager::getMouse()
{
	return mouse;
}

void GameInputManager::capture()
{
	keyboard->capture();
	mouse->capture();
}
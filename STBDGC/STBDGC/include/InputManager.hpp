#ifndef INPUT_MANAGER
#define INPUT_MANAGER

#include <OgreRenderWindow.h>
#include <OIS.h>

//Control the monster in FPS-like view WASD (ZQSD) + mouselook 
//E to make stuff happen

class GameInputManager
{
public:
	GameInputManager(Ogre::RenderWindow* inputSource);
	~GameInputManager();

	OIS::Keyboard* getKeyboard();
	OIS::Mouse* getMouse();

	//for event polling
	void capture();
private:
	OIS::Keyboard* keyboard;
	OIS::Mouse* mouse;
	OIS::InputManager* inputManager;
};


#endif
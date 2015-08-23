#ifndef LITTLE_BOY
#define LITTLE_BOY

#include <Ogre.h>

class LittleBoy
{
public:
	enum anims{idleSleep, surpriseJump};
	LittleBoy(Ogre::SceneNode* boy, Ogre::SceneManager* smgr);
	~LittleBoy();

	void addTime(float time);
	void play(anims whatToPlay);
private:

	Ogre::SceneNode* rootBoy;
	typedef std::vector<Ogre::AnimationState*> AnimationStateVect;
	AnimationStateVect *idle_sleep, *surprise_jump;
	AnimationStateVect* playing;
	float playTime;
	anims currentIdle;

};

#endif
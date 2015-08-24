#ifndef LITTLE_BOY
#define LITTLE_BOY

#include <Ogre.h>
#include <al.h>

class LittleBoy
{
public:
	enum anims{idleSleep, surpriseJump, idleCry};
	LittleBoy(Ogre::SceneNode* boy, Ogre::SceneManager* smgr);
	~LittleBoy();

	void addTime(float time);
	void play(anims whatToPlay);
	void changeIdleAnim(anims newIdle);
	void returnToSleep();

	bool isCrying();

	ALuint crySource;
	ALuint cryBuffer;
private:

	Ogre::SceneNode* rootBoy;
	typedef std::vector<Ogre::AnimationState*> AnimationStateVect;
	AnimationStateVect *idle_sleep, *surprise_jump, *idle_cry;
	AnimationStateVect* playing;
	float playTime;
	anims currentIdle;
	anims currentPlaying;
	bool reverse;

};

#endif
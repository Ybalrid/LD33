#include "pch.hpp"
#include "LittleBoy.hpp"

LittleBoy::LittleBoy(Ogre::SceneNode* boy, Ogre::SceneManager* smgr):
	playing(nullptr),
	playTime(0),
	reverse(false)
{
	rootBoy=boy;
	Ogre::Entity* tmpEntity;

	idle_sleep = new AnimationStateVect;
	surprise_jump = new AnimationStateVect;
	idle_cry = new AnimationStateVect;

	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_Eyebrow001.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_Eyelashes01.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_Low-Poly.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_Teeth_Base.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_male1591.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_mhair02.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_short01.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_tongue01.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));
	boy->attachObject(tmpEntity = smgr->createEntity("makehuman_tshirt02.mesh"));
	idle_sleep->push_back(tmpEntity->getAnimationState("idle_sleep"));
	idle_cry->push_back(tmpEntity->getAnimationState("idle_cry"));
	surprise_jump->push_back(tmpEntity->getAnimationState("surprise_jump"));

	for(Ogre::AnimationState* a : *idle_sleep) a->setLoop(true);
	for(Ogre::AnimationState* a : *idle_cry) a->setLoop(true);
	for(Ogre::AnimationState* a : *surprise_jump) a->setLoop(false);
	currentIdle = idleSleep;

}

void LittleBoy::addTime(float time)
{
	playTime += time;
	if(playing) for(Ogre::AnimationState* a : *playing) 
	{
		if(reverse)
			a->setTimePosition(a->getLength()-playTime);
		else
			a->addTime(time);
		//Return to idle animation when non-looping anim is finished
		if(!a->getLoop())
		{
			if(playTime > a->getLength())
			{
				play(currentIdle);
				reverse = false;
			}
		}
	}

}

void LittleBoy::play(anims whatToPlay)
{
	alSourcef(crySource, AL_GAIN, 0);
	//reset playtime
	playTime = 0;
	//disable current anim
	if(playing) for(Ogre::AnimationState* a : *playing)
		a->setEnabled(false);

	//Get the correct set of AnimationState
	switch(whatToPlay)
	{
	default:
		playing = nullptr;
		return;
	case idleSleep:
		playing = idle_sleep;
		break;
	case idleCry:
		alSourcef(crySource, AL_GAIN, 1);
		playing = idle_cry;
		break;
	case surpriseJump:
		playing = surprise_jump;
		break;
	}

	//enable and jump to time = 0
	if(playing) for(Ogre::AnimationState* a : *playing)
	{
		a->setEnabled(true);
		if(reverse)
			a->setTimePosition(a->getLength());
		else
			a->setTimePosition(0);
	}

	currentPlaying = whatToPlay;
}

void LittleBoy::changeIdleAnim(anims newIdle)
{
	switch(newIdle)
	{
	default:
	case idleSleep:
		currentIdle = idleSleep;
		break;
	case idleCry:
		currentIdle = idleCry;
		break;
	}
}

void LittleBoy::returnToSleep()
{
	changeIdleAnim(idleSleep);
	reverse = true;
	play(surpriseJump);
}

bool LittleBoy::isCrying()
{
	if(currentPlaying == idleCry)
		return true;
	return false;
}


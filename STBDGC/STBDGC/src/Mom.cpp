#include "pch.hpp"
#include "Mom.h"

Mom::Mom(Ogre::SceneNode* node, Ogre::SceneManager* smgr) 
	: walkAnim(nullptr), walkspeed(0.3)
{
	mom = node;

	/*mom_Eyebrow007.001.mesh    mom_eyelashes03.001.mesh  mom_proxy741.001.mesh
mom_HighPolyEyes.001.mesh  mom_fhair01.001.mesh
mom_Shirt01.001.mesh       mom_jeans01.001.mesh*/


	walkAnim = new std::vector<Ogre::AnimationState*>;
	Ogre::Entity* tmpEntity(nullptr);


	mom->attachObject(tmpEntity = smgr->createEntity("mom_Eyebrow007.001.mesh"));
	walkAnim->push_back(tmpEntity->getAnimationState("Walk"));
	mom->attachObject(tmpEntity = smgr->createEntity("mom_HighPolyEyes.001.mesh"));
	walkAnim->push_back(tmpEntity->getAnimationState("Walk"));
	mom->attachObject(tmpEntity = smgr->createEntity("mom_Shirt01.001.mesh"));	
	walkAnim->push_back(tmpEntity->getAnimationState("Walk"));
	mom->attachObject(tmpEntity = smgr->createEntity("mom_eyelashes03.001.mesh"));
	walkAnim->push_back(tmpEntity->getAnimationState("Walk"));
	mom->attachObject(tmpEntity = smgr->createEntity("mom_fhair01.001.mesh"));
	walkAnim->push_back(tmpEntity->getAnimationState("Walk"));
	mom->attachObject(tmpEntity = smgr->createEntity("mom_jeans01.001.mesh"));
	walkAnim->push_back(tmpEntity->getAnimationState("Walk"));
	mom->attachObject(tmpEntity = smgr->createEntity("mom_proxy741.001.mesh"));
	walkAnim->push_back(tmpEntity->getAnimationState("Walk"));

	for(Ogre::AnimationState* a : *walkAnim)
	{
		a->setEnabled(true);
		a->setLoop(true);
	}

	toCorridor = Ogre::Quaternion::IDENTITY;

	toBedRoom = Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y);
	goingToCorridor = true;
}

void Mom::addTime(float time)
{
	if(!walk) return;
	if(walkAnim) for(Ogre::AnimationState* a : *walkAnim)
		a->addTime(time);
	 mom->setPosition(mom->getPosition() + time * (mom->getOrientation()* (walkspeed*Ogre::Vector3(0,0,1))));
}

void Mom::goToBedroom()
{
	mom->setOrientation(toBedRoom);
	walk = true;
	goingToCorridor = false;
}

void Mom::goToCorridor()
{
	mom->setOrientation(toCorridor);
	walk = true;
	goingToCorridor = true;
}



bool Mom::notGoingToBedroom()
{
	return goingToCorridor;
}

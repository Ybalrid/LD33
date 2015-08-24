#ifndef MONSTER
#define MONSTER

#include <OgreVector2.h>
#include <al.h>
class Monster
{
public:
	Monster();
	float getHeight();
	Ogre::Vector2 getPlanarCoodinates();
	void setPlanarCoordinates(Ogre::Vector2);
	void setPlanarCoordinates(float x, float y);
	ALuint footstepBuffer;
	ALuint footstepSource;
	bool wasWalking;
	
private:
	float height;
	Ogre::Vector2 planarCoordinates;
};

#endif

#ifndef MONSTER
#define MONSTER

#include <OgreVector2.h>

class Monster
{
public:
	Monster();
	float getHeight();
	Ogre::Vector2 getPlanarCoodinates();
	void setPlanarCoordinates(Ogre::Vector2);
	
private:
	float height;
	Ogre::Vector2 planarCoordinates;
};

#endif

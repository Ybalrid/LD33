#include "pch.hpp"
#include "Monster.hpp"

Monster::Monster() : 
	height(1.7f)
{
	wasWalking = false;
}

float Monster::getHeight()
{
	return height;
}

void Monster::setPlanarCoordinates(Ogre::Vector2 p)
{
	planarCoordinates=p;
}

void Monster::setPlanarCoordinates(float x, float y)
{
	setPlanarCoordinates(Ogre::Vector2(x,y));
}

Ogre::Vector2 Monster::getPlanarCoodinates()
{
	return planarCoordinates;
}

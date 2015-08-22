#include "pch.hpp"
#include "Monster.hpp"

Monster::Monster() : 
	height(1.7f)
{

}

float Monster::getHeight()
{
	return height;
}

void Monster::setPlanarCoordinates(Ogre::Vector2 p)
{
	planarCoordinates=p;
}

Ogre::Vector2 Monster::getPlanarCoodinates()
{
	return planarCoordinates;
}
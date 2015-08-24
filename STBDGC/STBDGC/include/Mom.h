#ifndef MOM_inc
#define MOM_inc
#include <Ogre.h>
#include <vector>
class Mom
{
public:
		Mom(Ogre::SceneNode* node, Ogre::SceneManager* smgr);
		void addTime(float time);
		void goToBedroom();
		void goToCorridor();
		bool walk;		float walkspeed;
		bool notGoingToBedroom();
	
private:
		bool goingToCorridor;
		std::vector<Ogre::AnimationState*>* walkAnim;
		Ogre::SceneNode* mom;
		Ogre::Quaternion toBedRoom, toCorridor;
};

#endif
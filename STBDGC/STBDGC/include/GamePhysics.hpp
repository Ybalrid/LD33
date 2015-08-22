#ifndef GAME_PHYSICS
#define GAME_PHYSICS

//Bullet
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>

//btOgre
#include "BtOgrePG.h"
#include "BtOgreGP.h"
#include "BtOgreExtras.h"


class GamePhysics
{
public:
	GamePhysics(Ogre::SceneNode* rootNode);
	void step(unsigned long deltaTime);

	btDiscreteDynamicsWorld* getWorld();

private:
	btBroadphaseInterface* m_Broadphase;
	btDefaultCollisionConfiguration* m_CollisionConfiguration;
	btCollisionDispatcher* m_Dispatcher;
	btSequentialImpulseConstraintSolver* m_Solver;
	btGhostPairCallback* m_ghostPairCallback;

	//Bullet Dynamics World
	btDiscreteDynamicsWorld* m_DynamicsWorld;

	bool debugPhysics;
	BtOgre::DebugDrawer* m_debugDrawer;
};

#endif
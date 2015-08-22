#include "pch.hpp"
#include "GamePhysics.hpp"


GamePhysics::GamePhysics(Ogre::SceneNode* rootNode)
{
	//Initialize the Bullet world
	m_Broadphase = new btDbvtBroadphase();
	m_CollisionConfiguration = new btDefaultCollisionConfiguration();
	m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
	m_Solver = new btSequentialImpulseConstraintSolver();
	m_ghostPairCallback = new btGhostPairCallback();

	m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);

	m_DynamicsWorld->setGravity(btVector3(0,-9.81f,0));
	m_DynamicsWorld->getPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	debugPhysics = false;
	m_debugDrawer = new BtOgre::DebugDrawer(rootNode, m_DynamicsWorld);
	m_DynamicsWorld->setDebugDrawer(m_debugDrawer);

	Ogre::LogManager::getSingleton().logMessage("Physics engine GO!");
}

void GamePhysics::step(unsigned long deltaTime)
{
	if(debugPhysics)
		m_debugDrawer->step();

	m_DynamicsWorld->stepSimulation(float(deltaTime)/1000.0f);
}

btDiscreteDynamicsWorld* GamePhysics::getWorld()
{
	return m_DynamicsWorld;
}

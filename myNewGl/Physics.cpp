#include "stdafx.h"
#include "Physics.h"


//Physics::Physics()
//{
//}


//Physics::~Physics()
//{
//}

std::function<void(btRigidBody*)> rigidBodyDeleater(btDynamicsWorld* world)
{
	return [world](btRigidBody * body) {
		world->removeRigidBody(body);
		delete body;
	};
}

#pragma once
#include "stdafx.h"



class Physics
{
public:

	void symulate(float time) {
		dynamicsWorld->stepSimulation(time,10);
	}

	std::unique_ptr<btBroadphaseInterface> broadphase{ new btDbvtBroadphase() };
	std::unique_ptr<btDefaultCollisionConfiguration>collisionConfiguration{ new btDefaultCollisionConfiguration() };
	std::unique_ptr<btCollisionDispatcher> dispatcher{ new btCollisionDispatcher{ collisionConfiguration.get() } };
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver{ new btSequentialImpulseConstraintSolver() };
	std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld{ new btDiscreteDynamicsWorld(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get()) };

	Physics() {
		dynamicsWorld->setGravity(btVector3(0, -10.f, 0));
	}
private:

};

std::function<void(btRigidBody*)> rigidBodyDeleater(btDynamicsWorld* world);
#pragma once

#include "stdafx.h"
#include "Wraper.h"

#define PI 3.1415926f

struct Transformation;
class aabbColider;



struct CameraDirection {
	float xAngle = 0;
	float yAngle = 0;
};

using namespace entityx;

struct Camera {
	glm::mat4 projection = glm::perspective(glm::radians(60.f), 4.f/3.f, 0.01f, 200.f);;
	glm::mat4 view;
};

struct ProgramDescription;

struct CameraSystem : public System<CameraSystem> , public Receiver<CameraSystem> {


	ComponentHandle<Camera> camera;
	ComponentHandle<CameraDirection> direction;
	ComponentHandle<Position> position;
	const std::vector<ProgramDescription*> programs;

	CameraSystem(Entity camera, const std::vector<ProgramDescription*>& programs,  btDynamicsWorld* world, EntityX* ex);
	btDynamicsWorld* world;
	EntityX* ex;
	void configure(entityx::EventManager &event_manager);

	
	void receive(const MouseDownEvent &mouseDown);

	void receive(const MouseScrolEvent &mouseDown);

	glm::vec3 front;

	float scrollOffset = 0.f;
	bool wasMousePresed = false;
	Entity currentlyManipulatedEntity;
	const float speed = 10.f;
	const float sensitivity = 0.003f;

	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override;

	void manipulateObject(const glm::vec3& pos, const glm::vec3& dir, const TimeDelta& dt);

	void rayCast(const glm::vec3& start, const glm::vec3 direction, entityx::EntityManager &es);

	void updateLigthShader();

private:
	void setManipulationObject(Entity& e);
	void releaseManipulationObject();

};

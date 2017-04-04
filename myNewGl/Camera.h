#pragma once

#include "stdafx.h"

#define PI 3.1415926f

class Transformation;
class aabbColider;
struct Position {
	glm::vec3 pos = { 0,0,0 };
};
struct CameraDirection {
	float xAngle = 0;
	float yAngle = 0;
};

using namespace entityx;

struct Camera {
	glm::mat4 projection = glm::perspective(glm::radians(60.f), 4.f/3.f, 0.01f, 200.f);;
	glm::mat4 view;
};

struct CameraSystem : public System<CameraSystem> , public Receiver<CameraSystem> {

	void configure(entityx::EventManager &event_manager) {
		event_manager.subscribe <MouseDownEvent>(*this);
		event_manager.subscribe <MouseScrolEvent>(*this);

	}
	
	void receive(const MouseDownEvent &mouseDown) {
		if (mouseDown.code == GLFW_MOUSE_BUTTON_RIGHT) {
			wasMousePresed = true;
		}
	}
	void receive(const MouseScrolEvent &mouseDown) {
		scrollOffset += mouseDown.offset;
	}

	float scrollOffset = 0.f;
	bool wasMousePresed = false;
	Entity currentlyManipulatedEntity;
	const float speed = 10.f;
	const float sensitivity = 0.003f;

	void update(entityx::EntityManager &es, entityx::EventManager &events, TimeDelta dt) override;



	void manipulateObject(const glm::vec3& pos, const glm::vec3& dir, const TimeDelta& dt);


	void rayCast(const glm::vec3& start, const glm::vec3 direction, entityx::EntityManager &es);

};



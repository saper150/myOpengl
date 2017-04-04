#include "stdafx.h"
#include "Camera.h"
#include "Wraper.h"
#include "Colider.h"


void CameraSystem::update(entityx::EntityManager & es, entityx::EventManager & events, TimeDelta dt)
{

	ComponentHandle<Camera> cameraHandle;
	ComponentHandle<Position> positionHandle;
	ComponentHandle<CameraDirection> directionHandle;
	Entity cameraEntity = *es.entities_with_components(cameraHandle, positionHandle, directionHandle).begin();
	ComponentHandle<Camera> camera = cameraEntity.component<Camera>();
	ComponentHandle<Position> position = cameraEntity.component<Position>();
	ComponentHandle<CameraDirection> direction = cameraEntity.component<CameraDirection>();

	float& horizontalAngle = direction->xAngle;
	float& verdicalAngle = direction->yAngle;
	glm::vec3& cameraPos = position->pos;
	float time = dt;

	auto mouseDelta = Input::mousePositionDelta()*sensitivity;

	if (Input::leftMouseDown()) {
		verdicalAngle += mouseDelta.y;
		horizontalAngle += mouseDelta.x;
	}

	glm::vec3 _front = {
		cos(verdicalAngle) * sin(horizontalAngle),
		sin(verdicalAngle),
		cos(verdicalAngle) * cos(horizontalAngle),
	};
	_front = glm::normalize(_front);

	glm::vec3 rigth = {
		sin(horizontalAngle - PI / 2.f),
		0,
		cos(horizontalAngle - PI / 2.f)
	};
	glm::vec3 up = glm::cross(rigth, _front);

	if (Input::keyDown(GLFW_KEY_W)) {
		cameraPos += _front*time*speed;
	};
	if (Input::keyDown(GLFW_KEY_S)) {
		cameraPos += -_front*time*speed;
	};
	if (Input::keyDown(GLFW_KEY_A)) {
		cameraPos += -rigth*time*speed;
	};
	if (Input::keyDown(GLFW_KEY_D)) {
		cameraPos += rigth*time*speed;
	};
	if (Input::keyDown(GLFW_KEY_SPACE)) {
		cameraPos.y += time*speed;
	};
	if (Input::keyDown(GLFW_KEY_C)) {
		cameraPos.y -= time*speed;
	};
	camera->view = glm::lookAt(cameraPos, cameraPos + _front, up);

	if (wasMousePresed && currentlyManipulatedEntity.valid()) {
		currentlyManipulatedEntity.invalidate();
	}
	else if (wasMousePresed) {
		rayCast(cameraPos, _front, es);
	}
	manipulateObject(cameraPos, _front,dt);


	wasMousePresed = false;
	scrollOffset = 0.0f;
}

void CameraSystem::manipulateObject(const glm::vec3 & pos, const glm::vec3 & dir,const TimeDelta& dt)
{
	const float distance = 5.f;
	const float rotationVel = 1.6f;
	const float scrollVel = 0.07f;

	if (currentlyManipulatedEntity.valid()) {
		ComponentHandle<Transformation> transformation = currentlyManipulatedEntity.component<Transformation>();
		transformation->position = pos+(dir*distance);
		transformation->scale += scrollOffset*scrollVel;

		float rotateBy = rotationVel * dt;

		if (Input::keyDown(GLFW_KEY_UP)) {
			transformation->rotation *= glm::quat(cos(rotateBy / 2.f), sin(rotateBy / 2.f), 0, 0);
		}
		if (Input::keyDown(GLFW_KEY_LEFT)) {
			transformation->rotation *= glm::quat(cos(-rotateBy / 2.f), 0, sin(-rotateBy / 2.f), 0);

		}
		if (Input::keyDown(GLFW_KEY_DOWN)) {
			transformation->rotation *= glm::quat(cos(-rotateBy / 2.f), sin(-rotateBy / 2.f), 0, 0);

		}
		if (Input::keyDown(GLFW_KEY_RIGHT)) {
			transformation->rotation *= glm::quat(cos(rotateBy / 2.f), 0, sin(rotateBy / 2.f), 0);
		}



	}
}

void CameraSystem::rayCast(const glm::vec3 & start, const glm::vec3 direction, entityx::EntityManager & es)
{

	float minDistance = std::numeric_limits<float>::max();
	Entity closestEntity;
	es.each<Transformation, aabbColider*>(
		[&](Entity entity, Transformation &t, aabbColider* colider) {
		float distance;
		if (colider->testRay(start, direction, t.createModel(), distance)) {
			if (minDistance > distance) {
				minDistance = distance;
				closestEntity = entity;
			}
		}
	});
	if (minDistance != std::numeric_limits<float>::max()) {
		currentlyManipulatedEntity = closestEntity;
	}
}

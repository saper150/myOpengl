#pragma once

#include "stdafx.h"


#define PI 3.1415926f

struct Camera {
	Camera(GLFWwindow * window) :window(window)
	{
		updateAspectRation();
		updataProjection();
		updateView();

	}

	const glm::mat4& projection() const { return _projection; }
	const glm::mat4& view() const { return _view; }


	void setCameraPos(glm::vec3 cameraPos) {
		_cameraPos = cameraPos;
		updateView();
	}
	const glm::vec3& getCameraPos() const {
		return _cameraPos;
	}
	const glm::vec3& getCameraLookAt() const { return _cameraLookAt; }

	void setCameraLookAt(glm::vec3 cameraDir) {
		_cameraLookAt = cameraDir;
		updateView();
	}

	const glm::vec3 getCameraUp() const { return _cameraUp; }

	void setCameraUp(glm::vec3 cameraUp) {
		_cameraUp = cameraUp;
		updateView();
	}

	glm::vec3 cameraDirection() {
		return glm::normalize(_cameraLookAt - _cameraPos);
	}

	float getFow() const { return fow; }
	void setFow(float fow) {
		this->fow = fow;
		updataProjection();
	}

	void updateAspectRation() {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		this->aspectRatio = width / (float)height;
		updataProjection();
	}

	glm::mat4 _view;
private:
	void updataProjection() {
		_projection = glm::perspective(glm::radians(fow), aspectRatio, 0.01f, 100.f);

	}
	void updateView() {
		_view = glm::lookAt(_cameraPos, _cameraLookAt, _cameraUp);
	}

	float aspectRatio;
	float fow = 45.f;
	GLFWwindow * window;
	glm::mat4 _projection;
	glm::vec3 _cameraPos{ 0,0,5 };
	glm::vec3 _cameraLookAt{ 0,0,0 };
	glm::vec3 _cameraUp{ 0,1,0 };

};

class CameraControll {
	GLFWwindow* window;
	double basePositionX, basePositionY;

	glm::vec3 baseCameraDir;
	glm::vec3 cameraPos = {0,0,5};

	glm::vec2 mousePositionDelta() {
		static double prevx, prevy;
		static double posx, posy;

		prevx = posx;
		prevy = posy;

		glfwGetCursorPos(window, &posx, &posy);

		double deltax = prevx - posx;
		double deltay = prevy - posy;


		return{ deltax,deltay };

	}

public:


	CameraControll(GLFWwindow * window) :window(window)
	{

	}

	float mouseSensetivity = 0.003f;
	float movementSpeed = 10;
	void updataCamera(float time, Camera& camera) {
		static float horizontalAngle = 3.14f;
		static float verdicalAngle = 0.f;


		auto mouseDelta = mousePositionDelta()*mouseSensetivity;

		glm::vec3 direction = {
			cos(verdicalAngle) * sin(horizontalAngle),
			sin(verdicalAngle),
			cos(verdicalAngle) * cos(horizontalAngle),
		};
		direction = glm::normalize(direction);

		glm::vec3 rigth = {
			sin(horizontalAngle - PI / 2.f),
			0,
			cos(horizontalAngle - PI / 2.f)
		};
		glm::vec3 up = glm::cross(rigth, direction);
		

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			verdicalAngle += mouseDelta.y;
			horizontalAngle += mouseDelta.x;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cameraPos += direction*time*movementSpeed;
		};
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cameraPos += -direction*time*movementSpeed;
		};
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cameraPos += -rigth*time*movementSpeed;
		};
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cameraPos += rigth*time*movementSpeed;
		};
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			cameraPos += up*time*movementSpeed;
		};
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
			cameraPos += -up*time*movementSpeed;
		};

		camera._view = glm::lookAt(cameraPos, cameraPos + direction, up);

	}



};
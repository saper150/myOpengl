#pragma once

static class Input {
public:

	static void init(GLFWwindow *  window) {
		_window = window;
	}
	static bool leftMouseDown() {
		return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	}
	static bool rigthMouseDown() {
		return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	}


	static bool keyDown(int keyCode) {
		return glfwGetKey(_window, keyCode) == GLFW_PRESS;
	}

	static glm::vec2 mousePositionDelta() {

		static double prevx, prevy;
		static double posx, posy;

		prevx = posx;
		prevy = posy;

		glfwGetCursorPos(_window, &posx, &posy);

		double deltax = prevx - posx;
		double deltay = prevy - posy;
		return{ deltax,deltay };
	}

private:

	static GLFWwindow * _window;
	
};

struct MouseDownEvent
{
	int code;
};
struct MouseUpEvent
{
	int code;
};
struct MouseScrolEvent {
	float offset;
};

struct InputSystem : public entityx::System<InputSystem> {
private:
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static std::vector<int> clickActions;
	static std::vector<int> mouseButtonKeys;
	static float scrollOffset;
public:
	InputSystem(GLFWwindow* window)
	{
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetScrollCallback(window, scroll_callback);

	}
	void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;

};

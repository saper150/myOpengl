#include "stdafx.h"
#include "Input.h"

std::vector<int> InputSystem::clickActions;
std::vector<int> InputSystem::mouseButtonKeys;
float InputSystem::scrollOffset;

void InputSystem::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	clickActions.push_back(action);
	mouseButtonKeys.push_back(button);
}

void InputSystem::scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	scrollOffset += static_cast<float>(yoffset);
}

void InputSystem::update(entityx::EntityManager & es, entityx::EventManager & events, TimeDelta dt)
{
	for (size_t i = 0; i < clickActions.size(); i++) {
		if (clickActions[i] == GLFW_PRESS) {
			events.emit<MouseDownEvent>({mouseButtonKeys[i]});
		}
		else if (clickActions[i] == GLFW_RELEASE) {
			events.emit<MouseUpEvent>({ mouseButtonKeys[i] });
		}
	}
	if (scrollOffset != 0.f) {
		events.emit<MouseScrolEvent>({scrollOffset});
	}

	scrollOffset = 0.f;
	clickActions.clear();
	mouseButtonKeys.clear();
}

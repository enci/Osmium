#pragma once

#include <GLFW/glfw3.h>

namespace Osm
{

class Device
{
public:
	Device(GLFWwindow* window);

	int GetScreenWidth() const;

	int GetScreenHeight() const;

	GLFWwindow* GetWindow() { return _window; }

	bool GetKeyOnce(char key);

protected:
	int			_height;
	int			_width;
	char keyOnce[GLFW_KEY_LAST + 1];
	GLFWwindow* _window;
};

extern Device* pDevice;

}
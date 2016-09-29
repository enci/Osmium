#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core/Engine.h>

namespace Osm
{

class GraphicsDevice : public  Component<CEngine>
{
public:
	GraphicsDevice(CEngine& engine);

	virtual ~GraphicsDevice();

	int GetScreenWidth() const;

	int GetScreenHeight() const;

	GLFWwindow* GetWindow() { return _window; }

	bool GetKeyOnce(char key);

protected:
	int			_height = 0;
	int			_width = 0;
	float		_ratio;
	char		_keyOnce[GLFW_KEY_LAST + 1];
	GLFWwindow* _window = nullptr;
};

}
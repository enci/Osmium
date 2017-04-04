#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core/Game.h>

namespace Osm
{

class GraphicsDevice : public  Component<CGame>
{
public:
	GraphicsDevice(CGame& engine);

	virtual ~GraphicsDevice();

	int GetScreenWidth() const;

	int GetScreenHeight() const;

	GLFWwindow* GetWindow() const { return _window; }

protected:
	int			_height = 0;
	int			_width = 0;
	float		_ratio;
	GLFWwindow* _window = nullptr;
};

}
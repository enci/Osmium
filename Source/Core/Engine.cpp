#include <Core/Engine.h>
#include <Core/Resources.h>
#include <Core/World.h>
#include <Core/Device.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace Osm;

CEngine Osm::Engine;

void CEngine::Initialize()
{
	_initialized = true;

	_resources = CreateComponent<ResourceManager>();	

	// Load a config in the future maybe?

	_device = CreateComponent<GraphicsDevice>();
}

void CEngine::Shutdown()
{
	ASSERT(_initialized);

	// Remove component in the reverse order they were created
	while (_components.size() != 0)
		_components.erase(_components.end() - 1);
}

void CEngine::Run()
{
	ASSERT(_initialized);

	auto lastFrame = glfwGetTime();
	while (!glfwWindowShouldClose(_device->GetWindow()))
	{
		if (!_world)
			continue;

		auto currentFrame = glfwGetTime();
		auto deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime > 0.033f)
			deltaTime = 0.033f;
		_world->Update(deltaTime);

		glViewport(0, 0, _device->GetScreenWidth(), _device->GetScreenHeight());				
		_world->Render();
		glfwSwapBuffers(_device->GetWindow());
	}
}

void CEngine::SwapWorld(World* world)
{
	if (_world)
		delete _world;
	_world = world;
}

/*
void CEngine::Update()
{
	ASSERT(_initialized);
}

void CEngine::Render()
{
	ASSERT(_initialized);
}
*/
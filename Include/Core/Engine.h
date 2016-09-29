#pragma once

#include <Core/Component.h>

namespace Osm
{

class GraphicsDevice;
class ResourceManager;
class Inspector;
class World;

class CEngine : public ComponentContainer<CEngine>
{
public:

	void Initialize();

	void Shutdown();

	void Run();

	GraphicsDevice& Device() { return *_device; }

	ResourceManager& Resources() { return *_resources; }

	void SwapWorld(World* world);

protected:

//	void Update();

//	void Render();

protected:
	World* _world = nullptr;

	bool _initialized = false;

	GraphicsDevice* _device = nullptr;

	ResourceManager* _resources = nullptr;

	bool _paused;

	bool _advanceFrame;


};

extern CEngine Engine;

}
#pragma once

#include <Defines.h>
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

#ifdef INSPECTOR
	void Inspect();

	bool _show_engine_compoents;
	bool _show_input_debug;
	bool _show_world_inspector;
	bool _show_profiler;
#endif
};

extern CEngine Engine;

}
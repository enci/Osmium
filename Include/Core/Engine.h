#pragma once

#include <Defines.h>
#include <Core/Component.h>
#include <functional>

namespace Osm
{

class GraphicsDevice;
class ResourceManager;
class Inspector;
class Profiler;
class World;

class CEngine : public ComponentContainer<CEngine>
{
public:

	void Initialize();

	void Shutdown();

	void Run();

	GraphicsDevice& Device() { return *_device; }

	ResourceManager& Resources() { return *_resources; }

	Profiler& GetProfiler() { return *_profiler; }

	void SwapWorld(World* world);

	void QueueEvent(std::function<void(void)> e) { _event = e; }

protected:

//	void Update();

//	void Render();

protected:

	World* _world = nullptr;

	bool _initialized = false;

	GraphicsDevice* _device = nullptr;

	ResourceManager* _resources = nullptr;

	Profiler* _profiler = nullptr;

	bool _paused;

	bool _advanceFrame;

	std::function<void(void)> _event;

#ifdef INSPECTOR
	void Inspect();

	bool _show_engine_compoents = false;
	bool _show_input_debug = false;
	bool _show_world_inspector = false;
	bool _show_profiler = false;
	bool _show_imgui_test = false;
#endif
};

extern CEngine Engine;

}
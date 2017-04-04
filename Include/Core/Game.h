#pragma once

#include <Defines.h>
#include <Core/Component.h>
#include <functional>
#include <cereal/cereal.hpp>

namespace Osm
{

class GraphicsDevice;
class ResourceManager;
class Inspector;
class Profiler;
class World;
class InputManager;
class AudioManager;

struct GameSettings
{	
	void LoadFromFile(const std::string& file);

	template<class Archive>
	void serialize(Archive& archive);

#ifdef INSPECTOR
	void Inspect();
#endif


	int ScreenWidth = 1920;
	int ScreenHeight = 1080;
	bool FullScreen = false;
	bool UseNativeResolution = true;
	float InspectorFontSize = 1.0f;
	std::string ResourcePath = "";
	std::string SavePath = "";
	std::string WindowName = "Window";

protected:
	std::string FilePath;
	void SaveToFile();	
};

template <class Archive>
void GameSettings::serialize(Archive& archive)
{
	archive(
		CEREAL_NVP(ScreenWidth),
		CEREAL_NVP(ScreenHeight),
		CEREAL_NVP(FullScreen),
		CEREAL_NVP(UseNativeResolution),
		CEREAL_NVP(ResourcePath),
		CEREAL_NVP(InspectorFontSize),
		CEREAL_NVP(SavePath),
		CEREAL_NVP(WindowName)
	);
}

class CGame : public ComponentContainer<CGame>
{
public:	

	void Initialize(const GameSettings& options);

	void Shutdown();

	void Run();

	GraphicsDevice& Device() const { return *_device; }

	ResourceManager& Resources() const { return *_resources; }

	InputManager& Input() const { return *_input; }

	Profiler& GetProfiler() const { return *_profiler; }

	const GameSettings& Settings() const { return _settings; }

	void SwapWorld(World* world);

	void QueueEvent(std::function<void(void)> e) { _event = e; }

protected:
	void InitializeInternal();

//	void Update();

//	void Render();

protected:

	World* _world = nullptr;

	bool _initialized = false;

	GraphicsDevice* _device = nullptr;

	ResourceManager* _resources = nullptr;

	InputManager* _input = nullptr;

	Profiler* _profiler = nullptr;

	AudioManager* _audio = nullptr;

	GameSettings _settings;

	bool _paused = false;

	bool _advanceFrame = false;

	std::function<void(void)> _event;

#ifdef INSPECTOR
	void Inspect();

	bool _show_engine_compoents = false;
	bool _show_input_debug = false;
	bool _show_world_inspector = false;
	bool _show_profiler = false;
	bool _show_imgui_test = false;
	bool _show_settings = false;
	bool _close_inspector = false;
#endif
};

extern CGame Game;

}

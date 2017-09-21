#include <Core/Game.h>
#include <Core/Resources.h>
#include <Core/World.h>
#include <Core/Device.h>
#include <Input/Input.h>
#include <Audio/Audio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <imgui/IconsFontAwesome.h>
#include <Tools/Profiler.h>
#include <cereal/archives/json.hpp>
#include <fstream>

using namespace Osm;
using namespace std;

CGame Osm::Game;

#define SHOW_IMGUI 1

void GameSettings::LoadFromFile(const std::string& file)
{
	FilePath = file;
	ifstream fs;
	fs.open(FilePath);
	if (fs.is_open())
	{
		cereal::JSONInputArchive iarchive(fs);
		iarchive(cereal::make_nvp("Setttings", *this));
	}
	fs.close();
}

void GameSettings::SaveToFile()
{
	ofstream fs;
	fs.open(FilePath);

	{
		cereal::JSONOutputArchive oarchive(fs);
		oarchive(cereal::make_nvp("Setttings", *this));
	}
	fs.close();
}

void GameSettings::Inspect()
{
	ImGui::InputInt("Screen Width", &ScreenWidth, 320, 5600);
	ImGui::InputInt("Screen Height", &ScreenHeight, 240, 5600);
	ImGui::Checkbox("Full Screen", &FullScreen);
	ImGui::Checkbox("Native Resolution", &UseNativeResolution);
	ImGui::InputInt("MSAA Samples", &MSAASamples);
	ImGui::SliderFloat("Inspector Font Size", &InspectorFontSize, 0.5f, 2.0f);
	
	if (ImGui::Button("Save Settings"))
	{
		SaveToFile();
	}
}

void CGame::InitializeInternal()
{
	_initialized = true;

	_resources = CreateComponent<ResourceManager>();		

	_device = CreateComponent<GraphicsDevice>();

	_input = CreateComponent<InputManager>();

	_profiler = CreateComponent<Profiler>();

	_audio = CreateComponent<AudioManager>();

	ImGui_ImplGlfwGL3_Init(_device->GetWindow(), true);
}

void CGame::Initialize(const GameSettings& options)
{
	_settings = options;
	InitializeInternal();
	ImGui::SetUIStyle(true, 1.0f);
}

void CGame::Shutdown()
{
	ASSERT(_initialized);

	// Remove component in the reverse order they were created
	while (_components.size() != 0)
		_components.erase(_components.end() - 1);
}

void CGame::Run()
{
	ASSERT(_initialized);

	auto lastFrame = glfwGetTime();
	while (!glfwWindowShouldClose(_device->GetWindow()) ||
			glfwGetKey(_device->GetWindow(), GLFW_KEY_ESCAPE))
	{
		glfwPollEvents();

		if (_event)
		{
			_event();
			_event = nullptr;
		}

		if (!_world)
			continue;		

		auto currentFrame = glfwGetTime();
		float deltaTime = (float)(currentFrame - lastFrame);
		lastFrame = currentFrame;
		_time.WallTime += deltaTime;
		if (deltaTime > 0.033f)
			deltaTime = 0.033f;
		_time.DeltaTime = deltaTime;
		_time.ElapsedTime += deltaTime;

		// Update
		if (!_paused || _advanceFrame)
		{
			_profiler->StartFrame();
			uint updateID = _profiler->StartSection("Update");
			_input->Update();
			_world->Update(deltaTime);
			uint audioID = _profiler->StartSection("Audio");
			_audio->Update(deltaTime);
			_profiler->EndSection(audioID);
			_profiler->EndSection(updateID);
			_advanceFrame = false;
		}

		_resources->Update(deltaTime);


		uint renderID = _profiler->StartSection("Render");
		glViewport(0, 0, _device->GetScreenWidth(), _device->GetScreenHeight());				
		_world->Render();
		_profiler->EndSection(renderID);

		_profiler->EndFrame();

#ifdef INSPECTOR
		Inspect();
#endif

		glfwSwapBuffers(_device->GetWindow());

		// Escape hack
		auto joysticks = _input->GetActiveJoysticks();
		for (auto& j : joysticks)
		{
			if(_input->GetJoystickButtonPressed(j, JOYSTICK_BUTTON_BACK))
				return;
		}
	}
}

void CGame::SwapWorld(World* world)
{
	QueueEvent([this, world]()
	{
		if (_world)
			delete _world;
		_world = world;
	});
}

#ifdef INSPECTOR
void CGame::Inspect()
{		
	if (_input->GetKey(GLFW_KEY_RIGHT_SHIFT) && _input->GetKeyOnce(GLFW_KEY_I))
		_show_inspector = !_show_inspector;

	if (!_show_inspector)
		return;

	ImGui::GetIO().FontGlobalScale = _settings.InspectorFontSize;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui_ImplGlfwGL3_NewFrame();	
	bool p_open = true;

	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("Game", nullptr, &_show_engine_components);
		// Just a reminder that this might be useful
		// ImGui::MenuItem("Log", NULL, &show_app_log);
		ImGui::MenuItem("World", nullptr, &_show_world_inspector);
		ImGui::MenuItem("Settings", nullptr, &_show_settings);
		ImGui::MenuItem("Profiler", nullptr, &_show_profiler);
#if SHOW_IMGUI
		ImGui::MenuItem("ImGui Test", nullptr, &_show_imgui_test);
#endif
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("About"))
	{
		string timestamp = __TIMESTAMP__;
		ImGui::MenuItem(("Built on: " + timestamp).c_str());
		ImGui::MenuItem("Copyright (C) Bojan Endrovski 2017");		
		ImGui::EndMenu();
	}
	
	// Paused button
	if (!_paused)
		_paused = ImGui::Button(ICON_FA_PAUSE);
	else
		_paused = !ImGui::Button(ICON_FA_PLAY);
	ImGui::SameLine();
	_advanceFrame = ImGui::Button(ICON_FA_FAST_FORWARD);
	ImGui::SameLine();
	_show_inspector = !ImGui::Button(ICON_FA_TIMES);
	ImGui::SameLine();
	ImGui::LabelText("Frame Time", "");
	ImGui::SameLine();
	ImGui::ProgressBar((float)(_profiler->GetTimePerFrame() / (1.0 / 60.0)));	
	ImGui::EndMainMenuBar();


	if (_show_world_inspector)
	{
		ImGui::Begin("World Inspector", &_show_world_inspector);
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.4f);
		_world->Inspect();
		ImGui::PopItemWidth();
		ImGui::End();		
	}

	if(_show_profiler)
	{
		_profiler->Inspect();
	}

	if (_show_imgui_test)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&_show_imgui_test);
	}

	if(_show_engine_components)
	{		
		ImGui::Begin("Game Inspector", &_show_engine_components);
		for (auto& c : _components)
		{
			string name = typeid(*c).name();
			name = StringReplace(name, "class ", "");
			if (ImGui::CollapsingHeader(name.c_str()))
			{
				c->Inspect();
			}
		}
		ImGui::End();
	}

	if (_show_settings)
	{
		ImGui::Begin("Settings Inspector", &_show_settings);
		_settings.Inspect();
		ImGui::End();
	}	

	ImGui::Render();	
}
#endif


/*
void CGame::Update()
{
	ASSERT(_initialized);
}

void CGame::Render()
{
	ASSERT(_initialized);
}
*/
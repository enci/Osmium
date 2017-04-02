#include <Core/Engine.h>
#include <Core/Resources.h>
#include <Core/World.h>
#include <Core/Device.h>
#include <Input/Input.h>
#include <Audio/Audio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <Tools/Profiler.h>
#include <cereal/archives/json.hpp>
#include <fstream>

using namespace Osm;
using namespace std;

CEngine Osm::Engine;

void EngineSettings::LoadFromFile(const std::string& file)
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

void EngineSettings::SaveToFile()
{
	ofstream fs;
	fs.open(FilePath);

	{
		cereal::JSONOutputArchive oarchive(fs);
		oarchive(cereal::make_nvp("Setttings", *this));
	}
	fs.close();
}

void EngineSettings::Inspect()
{
	ImGui::SliderInt("Screen Width", &ScreenWidth, 320, 5600);
	ImGui::SliderInt("Screen Height", &ScreenHeight, 240, 5600);
	ImGui::Checkbox("Full Screen", &FullScreen);
	ImGui::Checkbox("Native Resolution", &UseNativeResolution);
	ImGui::SliderFloat("Inspector Font Size", &InspectorFontSize, 0.5f, 2.0f);

	if (ImGui::Button("Save Globals"))
	{
		SaveToFile();
	}
}

void CEngine::InitializeInternal()
{
	_initialized = true;

	_resources = CreateComponent<ResourceManager>();		

	_device = CreateComponent<GraphicsDevice>();

	_input = CreateComponent<InputManager>();

	_profiler = CreateComponent<Profiler>();

	_audio = CreateComponent<AudioManager>();

	ImGui_ImplGlfwGL3_Init(_device->GetWindow(), true);
}

void CEngine::Initialize(const EngineSettings& options)
{
	_settings = options;
	InitializeInternal();
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
		if (deltaTime > 0.033f)
			deltaTime = 0.033f;

		// Update
		if (!_paused || _advanceFrame)
		{
			_profiler->StartFrame();
			uint updateID = _profiler->StartSection("Update");
			_input->Update();
			_world->Update(deltaTime);
			_profiler->EndSection(updateID);
			_advanceFrame = false;
		}


		uint renderID = _profiler->StartSection("Render");
		glViewport(0, 0, _device->GetScreenWidth(), _device->GetScreenHeight());				
		_world->Render();
		_profiler->EndSection(renderID);

		_profiler->EndFrame();

#ifdef INSPECTOR
		Inspect();
#endif

		glfwSwapBuffers(_device->GetWindow());
	}
}

void CEngine::SwapWorld(World* world)
{
	QueueEvent([this, world]()
	{
		if (_world)
			delete _world;
		_world = world;
	});
}

#ifdef INSPECTOR
void CEngine::Inspect()
{
	ImGui::GetIO().FontGlobalScale = _settings.InspectorFontSize;

	ImGuiWindowFlags window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoTitleBar;	
	window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui_ImplGlfwGL3_NewFrame();

	bool p_open = true;

	ImGui::BeginMainMenuBar();
	{				
		{
			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Engine Inspector", nullptr, &_show_engine_compoents);
				// Just a reminder that this might be useful
				// ImGui::MenuItem("Console", NULL, &show_app_console);
				// ImGui::MenuItem("Log", NULL, &show_app_log);
				// ImGui::MenuItem("Input Debugger", nullptr, &_show_input_debug);
				ImGui::MenuItem("World Inspector", nullptr, &_show_world_inspector);				
				ImGui::MenuItem("Settings Inspector", nullptr, &_show_settings);
				ImGui::MenuItem("Profiler", nullptr, &_show_profiler);
				ImGui::MenuItem("ImGui Test", nullptr, &_show_imgui_test);
				ImGui::EndMenu();
			}		
		}	

		// Paused button
		if (!_paused)
			_paused = ImGui::Button("||");
		else
			_paused = !ImGui::Button("[>");
		ImGui::SameLine();
		_advanceFrame = ImGui::Button(">|");
		
		ImGui::SameLine();
		ImGui::LabelText("Frame Time", "");
		ImGui::SameLine();
		ImGui::ProgressBar(_profiler->GetTimePerFrame() / (1.0 / 60.0));
	}
	ImGui::EndMainMenuBar();

	if (_show_world_inspector)
	{
		ImGui::Begin("World Inspector");
		_world->Inspect();
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

	if(_show_engine_compoents)
	{		
		ImGui::Begin("Engine Inspector");
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
		ImGui::Begin("Settings Inspector");
		_settings.Inspect();
		ImGui::End();
	}

	ImGui::Render();
}
#endif


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
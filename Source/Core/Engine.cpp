#include <Core/Engine.h>
#include <Core/Resources.h>
#include <Core/World.h>
#include <Core/Device.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>
#include <Tools/Profiler.h>

using namespace Osm;

CEngine Osm::Engine;

void CEngine::Initialize()
{
	_initialized = true;

	_resources = CreateComponent<ResourceManager>();	

	// Load a config in the future maybe?

	_device = CreateComponent<GraphicsDevice>();

	_profiler = CreateComponent<Profiler>();

	ImGui_ImplGlfwGL3_Init(_device->GetWindow(), true);
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
		glfwPollEvents();

		if (!_world)
			continue;

		auto currentFrame = glfwGetTime();
		auto deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime > 0.033f)
			deltaTime = 0.033f;

		// Update
		_profiler->StartFrame();
		uint updateID = _profiler->StartSection("Update");
		_world->Update(deltaTime);
		_profiler->EndSection(updateID);


		uint renderID = _profiler->StartSection("Render");
		glViewport(0, 0, _device->GetScreenWidth(), _device->GetScreenHeight());				
		_world->Render();
		_profiler->EndSection(renderID);

#ifdef INSPECTOR
		Inspect();
#endif

		glfwSwapBuffers(_device->GetWindow());
	}
}

void CEngine::SwapWorld(World* world)
{
	if (_world)
		delete _world;
	_world = world;
}

#ifdef INSPECTOR
void CEngine::Inspect()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;	
	window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui_ImplGlfwGL3_NewFrame();

	bool p_open = true;

	if (ImGui::Begin("Osmium Inspector", &p_open, window_flags))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Engine Components", nullptr, &_show_engine_compoents);
				// Just a reminder that this might be useful
				// ImGui::MenuItem("Console", NULL, &show_app_console);
				// ImGui::MenuItem("Log", NULL, &show_app_log);
				ImGui::MenuItem("Input Debugger", nullptr, &_show_input_debug);
				ImGui::MenuItem("World Inspector", nullptr, &_show_world_inspector);
				ImGui::MenuItem("Profiler", nullptr, &_show_profiler);
				ImGui::MenuItem("ImGui Test", nullptr, &_show_imgui_test);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}

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
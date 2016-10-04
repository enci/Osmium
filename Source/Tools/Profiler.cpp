#include <Tools/Profiler.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <Graphics/Color.h>

using namespace Osm;
using namespace std;

vector<ImU32> colors =
{
	ImColor(Color::Magenta.integervalue),
	ImColor(Color::Yellow.integervalue),
	ImColor(Color::Green.integervalue),
	ImColor(Color::Cyan.integervalue),
	ImColor(Color::Orange.integervalue)
};

Profiler::Profiler(CEngine& engine)
	: Component(engine)
	, _sections()
	, _frameStart(-1.0f)
{
}

void Profiler::StartFrame()
{
	_frameStart = glfwGetTime();
}

uint Profiler::StartSection(const std::string& name)
{
	Section section;
	section.Name = name;
	section.StartTime = glfwGetTime() - _frameStart;
	section.EndTime = -1.0f;
	_sections.push_back(std::move(section));
	return _sections.size() - 1;
}

void Profiler::EndSection(uint sectionId)
{
	ASSERT(sectionId >= 0 && sectionId < _sections.size());
	auto& section = _sections[sectionId];
	section.EndTime = glfwGetTime() - _frameStart;
}

#ifdef INSPECTOR
void Profiler::Inspect()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	bool open;
	
	if (ImGui::Begin("Profiler"))
	{

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
						
		float spacing = 8.0f;
		float sz = 10.0f;
		float barOffset = 200;
		const ImVec2 basePos = ImGui::GetCursorScreenPos();
		float lenghtF = 1000 * 30;
		
		for (size_t i = 0; i < _sections.size(); i++)
		{
			ImVec2 p = ImGui::GetCursorScreenPos();

			ImU32 color = colors[i % colors.size()];

			auto& section = _sections[i];
			if (section.EndTime == -1.0f)
				continue;

			double time = section.EndTime - section.StartTime;	

			ImGui::TextColored(ImColor(color), "%s - %f", section.Name.c_str(), time * 1000);

			float x0 = p.x + (int)(section.StartTime * lenghtF) + barOffset;
			float x1 = p.x + (int)(section.EndTime * lenghtF) + barOffset;

			draw_list->AddRectFilled(ImVec2(x0, p.y + 4), ImVec2(x1, p.y + sz), color);

			p.y += sz + spacing;
			ImGui::SetCursorScreenPos(p);

			//p.y += sz + spacing;

			//x = p.x + 4;

			// time *= 1000;
			// ImGui::Text("Section: %s took: %f", section.Name.c_str(), time);

			// ImGui::

			// ImGui::ProgressBar()
			// section.EndTime = glfwGetTime();
		}

		float lowY = ImGui::GetCursorScreenPos().y;
		ImVec2 p = ImGui::GetCursorScreenPos();

		float max = 1.0f / 60.0f;
		draw_list->AddRectFilled(	ImVec2(p.x + max * lenghtF + barOffset, 0),
									ImVec2(p.x + max * lenghtF + barOffset + 2, lowY),
									ImColor(Color::White.integervalue));

		ImGui::End();
	}

	_sections.clear();
}
#endif

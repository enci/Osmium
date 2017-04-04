#include <imgui.h>
#include <Graphics/Color.h>

void ImGui::OsmColor(const char* label, Osm::Color& color, bool showAplha)
{
	float c[4] = {
		color.r / 255.0f,
		color.g / 255.0f,
		color.b / 255.0f,
		color.a / 255.0f
	};
	ImGui::ColorEdit4(label, c, showAplha);
	color.r = (char)(c[0] * 255);
	color.g = (char)(c[1] * 255);
	color.b = (char)(c[2] * 255);
	color.a = (char)(c[3] * 255);
}
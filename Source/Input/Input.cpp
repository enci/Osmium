#include <Input/Input.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <string>

using namespace Osm;
using namespace std;

InputManager::InputManager(CEngine& engine)
	: Component(engine)
	, _joysticks(GLFW_JOYSTICK_LAST)
{}

#ifdef INSPECTOR
void InputManager::Inspect()
{
	for (int joy = GLFW_JOYSTICK_1; joy < GLFW_JOYSTICK_LAST; joy++)
	{
		int present = glfwJoystickPresent(joy);
		if(present)
		{
			string name(glfwGetJoystickName(joy));
			name += " - " + to_string(joy);
			if (ImGui::CollapsingHeader(name.c_str()))
			{
				int count;
				JoystickState& state = _joysticks[joy];
				
				const float* axes = glfwGetJoystickAxes(joy, &count);
				for (size_t i = 0; i < count; i++)
				{
					state.Axes[i] = axes[i];
					ImGui::DragFloat("Axis", &state.Axes[i]);
				}

				const unsigned char* buttons = glfwGetJoystickButtons(joy, &count);
				for (size_t i = 0; i < count; i++)
				{
					state.Buttons[i] = buttons[i];
					ImGui::DragInt("Buttons", &state.Buttons[i]);
				}
			}
		}
	}
}
#endif
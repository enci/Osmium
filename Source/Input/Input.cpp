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

void InputManager::Update()
{
	_joyCount = 0;
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++)
	{
		int present = glfwJoystickPresent(i);

		if (present)
		{
			//string name(glfwGetJoystickName(i));			
			
			int count;
			JoystickState& state = _joysticks[_joyCount];
			//state.Name = name;

			const float* axes = glfwGetJoystickAxes(i, &count);
			for (size_t j = 0; j < count; j++)
			{
				state.Axes[j] = axes[j];
			}

			const unsigned char* buttons = glfwGetJoystickButtons(i, &count);
			for (size_t j = 0; j < count; j++)
			{
				state.Buttons[j] = buttons[j];
			}

			_joyCount++;
		}
	}
}

bool InputManager::GetJoystickButton(uint joystick, JoystickButtons button)
{
	if (joystick < _joysticks.size())
		return _joysticks[joystick].Buttons[button];
	return false;
}

float InputManager::GetJoystickAxis(uint joystick, JoystickAxes axis)
{
	if (joystick < _joysticks.size())
		return _joysticks[joystick].Axes[axis];
	return 0.0f;
}

#ifdef INSPECTOR
void InputManager::Inspect()
{
	for (auto j : _joysticks)
	{
		if (ImGui::CollapsingHeader(j.Name.c_str()))
		{
			for (auto a : j.Axes)		
				ImGui::DragFloat("Axis", &a);		

			for(auto b : j.Buttons)
				ImGui::DragInt("Buttons", &b);
		}
	}
}
#endif
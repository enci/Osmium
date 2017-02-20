#include <Input/Input.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <string>

using namespace Osm;
using namespace std;

void joystick_callback(int joy, int event)
{
	if (event == GLFW_CONNECTED)
	{
		Engine.Input().AddJoystick(joy);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		Engine.Input().RemoveJoystick(joy);
	}
}

InputManager::InputManager(CEngine& engine)
	: Component(engine)
	, _joyState(0)
{
	Init();
	glfwSetJoystickCallback(joystick_callback);
}

void InputManager::Init()
{
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++)
	{
		int present = glfwJoystickPresent(i);
		if(present)
		{
			_joyMapping.push_back(i);

			JoystickState state;
			string name(glfwGetJoystickName(i));            			
			state.Name = name + " " + to_string(i);
			_joyState.push_back(state);
		}
	}
}

void InputManager::Update()
{	
	for (int i = 0; i < _joyMapping.size(); i++)
	{		
		int joy = _joyMapping[i];
		JoystickState& state = _joyState[i];			
		int count;
				
		const float* axes = glfwGetJoystickAxes(joy, &count);
		for (size_t j = 0; j < count && j < JOYSTICK_AXIS_COUNT; j++)
		{
			state.Axes[j] = axes[j];
		}

		const unsigned char* buttons = glfwGetJoystickButtons(joy, &count);
		for (size_t j = 0; j < count && JOYSTICK_BUTTON_COUNT; j++)
		{
			state.Buttons[j] = buttons[j];
		}
	}
}

bool InputManager::GetJoystickButton(uint joystick, JoystickButtons button)
{
	if (joystick < _joyState.size())
	{
		return _joyState[joystick].Buttons[button];
	}
	return false;
}

float InputManager::GetJoystickAxis(uint joystick, JoystickAxes axis)
{
	if (joystick < _joyState.size())
	{
		return _joyState[joystick].Axes[axis];
	}
	return 0.0f;
}

void InputManager::AddJoystick(int joy)
{
	int g = 0;
}

void InputManager::RemoveJoystick(int joy)
{
	int g = 0;
}

#ifdef INSPECTOR
void InputManager::Inspect()
{
	for (auto j : _joyState)
	{
		if (ImGui::CollapsingHeader(j.Name.c_str()))
		{
			int i = 0;
			for (auto a : j.Axes)
			{
				ImGui::DragFloat((string("Axis ") + to_string(i)).c_str(), &a);
				i++;
			}

			i = 0;
			for (auto b : j.Buttons)
			{
				ImGui::DragInt((string("Buttons ") + to_string(i)).c_str(), &b);
				i++;
			}
		}
	}
}
#endif

void InputManager::InitProfiles()
{
	/*
	Profile xboxOne;
	xboxOne.Buttons		= 
	{
		0,		// JOYSTICK_BUTTON_A
		1,		// JOYSTICK_BUTTON_B
		2,		// JOYSTICK_BUTTON_X
		3,		// JOYSTICK_BUTTON_Y
		// JOYSTICK_BUTTON_LB
		// JOYSTICK_BUTTON_RB
		// JOYSTICK_BUTTON_BACK
		// JOYSTICK_BUTTON_START
		// JOYSTICK_BUTTON_DPAD_UP
		// JOYSTICK_BUTTON_DPAD_DOWN
		// JOYSTICK_BUTTON_DPAD_LEFT
		// JOYSTICK_BUTTON_DPAD_RIGHT
		// JOYSTICK_BUTTON_R3
		// JOYSTICK_BUTTON_L3
	};
	xboxOne.Axes		= {0, 1,  5, 4};
	*/
}
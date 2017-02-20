#pragma once

#include <Core/Component.h>
#include <Core/Engine.h>

namespace Osm
{

struct JoystickState
{
	std::string			Name;
	std::vector<float>	Axes;
	std::vector<int>	Buttons;

	JoystickState() : Axes(8), Buttons(24) {}
};

enum JoystickButtons
{
	JOYSTICK_BUTTON_A = 0,
	JOYSTICK_BUTTON_B,
	JOYSTICK_BUTTON_X,
	JOYSTICK_BUTTON_Y,
	JOYSTICK_BUTTON_RB,
	JOYSTICK_BUTTON_RT,
	JOYSTICK_BUTTON_LB,
	JOYSTICK_BUTTON_LT,
	JOYSTICK_BUTTON_BACK,
	JOYSTICK_BUTTON_START,
	JOYSTICK_BUTTON_DPAD_UP,
	JOYSTICK_BUTTON_DPAD_DOWN,
	JOYSTICK_BUTTON_DPAD_LEFT,
	JOYSTICK_BUTTON_DPAD_RIGHT,
	JOYSTICK_BUTTON_R3,
	JOYSTICK_BUTTON_L3,
};

enum JoystickAxes
{
	JOYSTICK_AXIS_RIGHT_THUMB_HORIZONTAL = 0,
	JOYSTICK_AXIS_RIGHT_THUMB_VERTICAL,
	JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL,
	JOYSTICK_AXIS_LEFT_THUMB_VERTICAL,
	JOYSTICK_AXIS_RIGHT_TRIGGER,
	JOYSTICK_AXIS_LEFT_TRIGGER,
};

class InputManager : public Component<CEngine>
{
public:
	InputManager(CEngine& engine);

	~InputManager() {}

	void Update();

	bool GetJoystickButton(uint joystick, JoystickButtons button);

	float GetJoystickAxis(uint joystick, JoystickAxes axis);

	uint GetJoystickCount() const { return _joyCount; }

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

private:
	std::vector<JoystickState> _joysticks;
	uint _joyCount;
};

}
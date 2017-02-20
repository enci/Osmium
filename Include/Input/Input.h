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
	//int					Profile;

	JoystickState() : Axes(8), Buttons(24) {}
};

enum JoystickButtons
{
	// Defualt order matching Xbox (360 and One) controller
	JOYSTICK_BUTTON_A = 0,
	JOYSTICK_BUTTON_B,				// 1
	JOYSTICK_BUTTON_X,				// 2
	JOYSTICK_BUTTON_Y,				// 3
	JOYSTICK_BUTTON_LB,				// 4
	JOYSTICK_BUTTON_RB,				// 5	
	JOYSTICK_BUTTON_BACK,			// 6
	JOYSTICK_BUTTON_START,			// 7
	JOYSTICK_BUTTON_L3,				// 8
	JOYSTICK_BUTTON_R3,				// 9
	JOYSTICK_BUTTON_DPAD_UP,		// 10
	JOYSTICK_BUTTON_DPAD_RIGHT,		// 11
	JOYSTICK_BUTTON_DPAD_DOWN,		// 12
	JOYSTICK_BUTTON_DPAD_LEFT,		// 13			
	JOYSTICK_BUTTON_COUNT
};

enum JoystickAxes
{
	// Defualt order matching Xbox (360 and One) controller
	JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL	= 0,
	JOYSTICK_AXIS_LEFT_THUMB_VERTICAL,			// 1
	JOYSTICK_AXIS_RIGHT_THUMB_HORIZONTAL,		// 2
	JOYSTICK_AXIS_RIGHT_THUMB_VERTICAL,			// 3
	JOYSTICK_AXIS_LEFT_TRIGGER,					// 4
	JOYSTICK_AXIS_RIGHT_TRIGGER,				// 5	
	JOYSTICK_AXIS_COUNT							// 6
};

struct Profile
{
	std::vector<int>	Axes;
	std::vector<int>	Buttons;

	Profile()
		: Axes(JOYSTICK_AXIS_COUNT)
		, Buttons(JOYSTICK_BUTTON_COUNT) {}
};

class InputManager : public Component<CEngine>
{
public:
	InputManager(CEngine& engine);

	~InputManager() {}

	void Init();

	void InitProfiles();

	void Update();

	bool GetJoystickButton(uint joystick, JoystickButtons button);

	float GetJoystickAxis(uint joystick, JoystickAxes axis);

	uint GetJoystickCount() const { return _joyMapping.size(); }

	void AddJoystick(int joy);

	void RemoveJoystick(int joy);

#ifdef INSPECTOR
	virtual void Inspect() override;
	
#endif

private:
	std::vector<Profile>		_profiles;
	std::vector<JoystickState>	_joyState;
	std::vector<int>			_joyMapping;
};

}
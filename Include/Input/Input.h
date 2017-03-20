#pragma once

#include <Core/Component.h>
#include <Core/Engine.h>
#include <unordered_map>

namespace Osm
{

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

class InputManager;

class Joystick
{
	friend class InputManager;
public:
	Joystick()				{ _joystick = -1; }
	bool IsValid() const	{ return _joystick != -1;  }

private:	
	Joystick(int j)			{ _joystick = (int)j; }
	operator int() const	{ return _joystick; }
	int _joystick;
};

class InputManager : public Component<CEngine>
{
private:

	struct JoystickState
	{
		//int					Joystick;		// Implementation ID of the joystick 
		std::string			Name;			// The name of it
		std::vector<float>	Axes;			// All the axes
		std::vector<int>	Buttons;		// All the buttons		
		std::vector<float>	LastAxes;		// All the axes from previous update
		std::vector<int>	LastButtons;	// All the buttons from previous update
		int					Profile;		// The profile (default -1 = Xbox)

		JoystickState()
			: /*Joystick(-1),*/ Axes(8), Buttons(24)
			, LastAxes(8), LastButtons(24), Profile(-1) {}
	};

public:
	InputManager(CEngine& engine);

	~InputManager() {}

	void Init();

	void InitProfiles();

	void Update();

	/// True if down 
	bool GetJoystickButton(Joystick joystick, JoystickButtons button);

	/// True only on first press
	bool GetJoystickButtonPressed(Joystick joystick, JoystickButtons button);

	float GetJoystickAxis(Joystick joystick, JoystickAxes axis);

	std::vector<Joystick> GetActiveJoysticks() const;

	void AddJoystick(int joy);

	void RemoveJoystick(int joy);

#ifdef INSPECTOR
	virtual void Inspect() override;

	void AddVirtualJoystick();
#endif

private:
	uint									_count;
	std::vector<Profile>					_profiles;
	std::unordered_map<int, JoystickState>	_joyState;	// Indexed by tokens
	int										_nextVirtual = 256;
};

}
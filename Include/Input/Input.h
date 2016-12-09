#pragma once

#include <Core/Component.h>
#include <Core/Engine.h>

namespace Osm
{

struct JoystickState
{
	std::vector<float>	Axes;
	std::vector<int>	Buttons;

	JoystickState() : Axes(8), Buttons(24) {}
};

class InputManager : public Component<CEngine>
{
public:
	InputManager(CEngine& engine); // : Component(engine) {}

	~InputManager() {}

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

private:
	std::vector<JoystickState> _joysticks;
};

}
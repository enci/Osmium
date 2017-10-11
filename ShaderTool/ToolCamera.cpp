#include "ToolCamera.h"
#include <Core/Game.h>
#include <Core/Device.h>

using namespace Osm;

ToolCamera::ToolCamera()
{
	_transform = CreateComponent<Transform>();
	_camera = CreateComponent<Camera>();
}

void ToolCamera::Update(float dt)
{		
	const float r = 2.5f;
	float dx = 0.0f;
	float dy = 0.0f;
	float speed = 0.01f;
	if(glfwGetMouseButton(Game.Device().GetWindow(), GLFW_MOUSE_BUTTON_1))
	{
		if(_wasMouseDown)
		{
			double newMouseX, newMouseY;
			glfwGetCursorPos(Game.Device().GetWindow(), &newMouseX, &newMouseY);
			dx = (float)(newMouseX - _mouseX) * speed;
			dy = (float)(newMouseY - _mouseY) * speed;
			_mouseX = newMouseX;
			_mouseY = newMouseY;			
		}
		else
		{
			glfwGetCursorPos(Game.Device().GetWindow(), &_mouseX, &_mouseY);
			_wasMouseDown = true;
		}		
	}
	else
	{
		_wasMouseDown = false;
	}

	_theta -= dx;

	float x = r * sin(_theta);
	float y = 1.4f;
	float z = r * cos(_theta);

	float fov = 60.0f;
	int screenWidth = Game.Device().GetScreenWidth();
	int screenHeight = Game.Device().GetScreenHeight();
	float ratio = float(screenWidth) / float(screenHeight);

	Matrix44 view = Matrix44::CreateLookAt(Vector3(x, y, z), Vector3(), Vector3(0, 1, 0));
	Matrix44 projection = Matrix44::CreatePerspective(DegToRad(fov), ratio, 0.1f, 4000.0f);

	_camera->SetView(view);
	_camera->SetProjection(projection);
}

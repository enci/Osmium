#include "ToolCamera.h"
#include <Core/Game.h>
#include <Core/Device.h>

using namespace Osm;

ToolCamera::ToolCamera(World& world) : Entity(world)
{
	_transform = CreateComponent<Transform>();
	_camera = CreateComponent<Camera>();

	float fov = 60.0f;
	//float dist = _world->CameraStartDistance;
	//_averagePosition = Vector3(0, dist, 0);
	int screenWidth = Game.Device().GetScreenWidth();
	int screenHeight = Game.Device().GetScreenHeight();
	float ratio = float(screenWidth) / float(screenHeight);

	Matrix44 view = Matrix44::CreateLookAt(Vector3(3.0f, 3.0f, 3.0f), Vector3(), Vector3(0, 0, 1));
	Matrix44 projection = Matrix44::CreatePerspective(DegToRad(fov), ratio, 0.1f, 4000.0f);

	_camera->SetView(view);
	_camera->SetProjection(projection);
}

void ToolCamera::Update(float dt)
{
}

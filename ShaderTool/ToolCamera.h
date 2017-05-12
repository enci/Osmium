#pragma once

#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Graphics/Render.h>


class ToolCamera : public Osm::Entity
{
public:

	ToolCamera(Osm::World& world);

	void  Update(float dt)  override;

protected:
	Osm::Transform*	_transform = nullptr;
	Osm::Camera*	_camera = nullptr;
};

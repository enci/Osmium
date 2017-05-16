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
	Osm::Transform*	_transform	= nullptr;
	Osm::Camera*	_camera		= nullptr;
	double			_mouseX;
	double			_mouseY;
	float			_fi = 0.0;
	float			_theta		= Osm::Pi * 0.25f;
	bool			_wasMouseDown = false;
};

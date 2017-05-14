#pragma once

#include <Core/World.h>
#include "ToolCamera.h"


class ToolWorld : public Osm::World
{
public:
	ToolWorld();

	// virtual void Update(float dt) override;

	virtual void Render() override;

#ifdef INSPECTOR
	void Inspect() override;
#endif

protected:
	Osm::RenderManager*			_renderManager	= nullptr;
	ToolCamera*					_camera			= nullptr;
	bool						_wireframe		= false;
};

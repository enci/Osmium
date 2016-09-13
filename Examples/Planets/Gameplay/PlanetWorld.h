#pragma once

#include <Core/World.h>
#include <Math/Matrix44.h>
#include <Graphics/DebugRenderer.h>
#include <Graphics/Mesh.h>
#include <Core/Transform.h>


class Celesital : public Osm::Entity
{
public:
	Celesital(Osm::World& world);

	virtual void Update(float dt) override;

private:
	//igad::Transform*	_transform;
	float rot = 0;
	float vel;
	float dist = -1.0f;
};

class PlanetWorld : public Osm::World
{
public:
	PlanetWorld();

	void Render() override;

protected:
	Osm::Matrix44	_view;
	Osm::Matrix44	_projection;

	Osm::Texture*	_texture;
	Osm::Mesh*		_mesh;
};
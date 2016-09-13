#pragma once

#include <Core/World.h>
#include <Math/Matrix44.h>
#include <Graphics/DebugRenderer.h>
#include <Graphics/Mesh.h>
#include <Core/Transform.h>


class Celesital : public igad::Entity
{
public:
	Celesital(igad::World& world);

	virtual void Update(float dt) override;

private:
	//igad::Transform*	_transform;
	float rot = 0;
	float vel;
	float dist = -1.0f;
};

class PlanetWorld : public igad::World
{
public:
	PlanetWorld();

	void Render() override;

protected:
	igad::Matrix44	_view;
	igad::Matrix44	_projection;

	igad::Texture*	_texture;
	igad::Mesh*		_mesh;
};
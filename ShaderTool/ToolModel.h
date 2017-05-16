#pragma once

#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Graphics/MeshRenderer.h>

class ToolModel : public Osm::Entity
{
	enum Model
	{
		Sphere		= 0,
		Plane		= 1,
		Monkey		= 2
	};

public:
	ToolModel(Osm::World& world);

	void Update(float dt) override;

#ifdef INSPECTOR
	void Inspect() override;
#endif

private:
	void Setup();

	Model _model				= Model::Sphere;
	bool _dirty					= false;
	int _sphereTesselation		= 4;
	int _planeResolution		= 10;
	float _size					= 1.0f;

	Osm::Transform* _transform		= nullptr;
	Osm::Mesh* _mesh				= nullptr;
	Osm::MeshRenderer* _renderer	= nullptr;
	Osm::Mesh* _teapot				= nullptr;
};
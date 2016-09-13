#pragma once
#include <Core/Component.h>
#include <Core/World.h>
#include <Graphics/Shader.h>
#include <Graphics/Mesh.h>

class PlanetsAssets : public Osm::Component<Osm::World>
{
public:
	PlanetsAssets(Osm::World& world);
	~PlanetsAssets();

	Osm::Shader*	BasicShader				= nullptr;
	Osm::Texture*	WhiteTexture			= nullptr;
	Osm::Texture*	GreyTexture				= nullptr;
	Osm::Mesh*		GeoSpheres[6];
};


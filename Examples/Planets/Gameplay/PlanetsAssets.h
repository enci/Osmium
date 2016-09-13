#pragma once
#include <Core/Component.h>
#include <Core/World.h>
#include <Graphics/Shader.h>
#include <Graphics/Mesh.h>

class PlanetsAssets : public igad::Component<igad::World>
{
public:
	PlanetsAssets(igad::World& world);
	~PlanetsAssets();

	igad::Shader*	BasicShader				= nullptr;
	igad::Texture*	WhiteTexture			= nullptr;
	igad::Texture*	GreyTexture				= nullptr;
	igad::Mesh*		GeoSpheres[6];
};


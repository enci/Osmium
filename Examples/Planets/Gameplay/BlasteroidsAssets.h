#pragma once
#include <Core/Component.h>
#include <Core/World.h>
#include <Graphics/Shader.h>
#include <Graphics/Mesh.h>


class BlasteroidsAssets : public igad::Component<igad::World>
{
public:
	BlasteroidsAssets(igad::World& world);
	~BlasteroidsAssets();

	igad::Shader*	AsteroidShader			= nullptr;
	igad::Mesh*		AsteroidMesh			= nullptr;
	igad::Texture*	WhiteTexture			= nullptr;
	igad::Texture*	GreyTexture				= nullptr;
	igad::Mesh*		AsteroidBlasterMesh		= nullptr;
	igad::Mesh*		BlasterMarkingsMesh		= nullptr;
	igad::Mesh*		AsteroidBaseMesh		= nullptr;
	igad::Mesh*		BoxMesh					= nullptr;
	igad::Mesh*		PlanetMesh				= nullptr;
	igad::Shader*	ParticleShader			= nullptr;
	igad::Shader*	DustShader				= nullptr;
	igad::Mesh*		AmmoMesh				= nullptr;
	igad::Mesh*		AmmoCoreMesh			= nullptr;
	igad::Mesh*		CrystalMesh				= nullptr;
	igad::Mesh*		GeoSpheres[6];
};


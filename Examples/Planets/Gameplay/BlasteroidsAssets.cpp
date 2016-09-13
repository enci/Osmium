
#include "BlasteroidsAssets.h"
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>

using namespace igad;

BlasteroidsAssets::BlasteroidsAssets(World& world) : Component(world)
{
	AsteroidShader = new Shader("./Assets/Shaders/Basic.vsh",
								"./Assets/Shaders/Basic.fsh");
	ParticleShader = new Shader("./Assets/Shaders/Particles.vsh",
								"./Assets/Shaders/Particles.gsh",
								"./Assets/Shaders/Particles.fsh");
	DustShader = new Shader("./Assets/Shaders/StarDust.vsh",
							"./Assets/Shaders/StarDust.gsh",
							"./Assets/Shaders/StarDust.fsh");

	//AsteroidMesh = new Mesh("./Assets/Models/Sphere.obj");

	AsteroidMesh = new Mesh("./Assets/Models/GeoSphere8.obj");

	BoxMesh = new Mesh("./Assets/Models/Box.obj");
	PlanetMesh = new Mesh("./Assets/Models/Planet.obj");
	WhiteTexture = new Texture("./Assets/Textures/white.png");
	GreyTexture = new Texture("./Assets/Textures/darkgrey.png");
	AsteroidBlasterMesh = new Mesh("./Assets/Models/AsteroidBlaster2.obj");
	BlasterMarkingsMesh = new Mesh("./Assets/Models/BlasterPlayerMarkings.obj");
	AmmoMesh = new Mesh("./Assets/Models/Ammo.obj");
	AmmoCoreMesh = new Mesh("./Assets/Models/AmmoCore.obj");

	// Load but don't apply
	AsteroidBaseMesh = new Mesh();
	AsteroidBaseMesh->Load("./Assets/Models/GeoSphere8.obj");


	GeoSpheres[0] = new Mesh();
	GeoSpheres[0]->Load("./Assets/Models/GeoSphere8.obj");

	GeoSpheres[1] = new Mesh();
	GeoSpheres[1]->Load("./Assets/Models/GeoSphere16.obj");

	GeoSpheres[2] = new Mesh();
	GeoSpheres[2]->Load("./Assets/Models/GeoSphere20.obj");

	GeoSpheres[3] = new Mesh();
	GeoSpheres[3]->Load("./Assets/Models/GeoSphere32.obj");

	GeoSpheres[4] = new Mesh();
	GeoSpheres[4]->Load("./Assets/Models/GeoSphere64.obj");

	GeoSpheres[5] = new Mesh();
	GeoSpheres[5]->Load("./Assets/Models/GeoSphere80.obj");

	CrystalMesh = new Mesh();
	CrystalMesh->Load("./Assets/Models/Crystal.obj");
}

BlasteroidsAssets::~BlasteroidsAssets()
{
	delete AsteroidShader;
	delete ParticleShader;
	delete DustShader;

	delete AsteroidMesh;
	delete BoxMesh;
	delete PlanetMesh;
	delete WhiteTexture;
	delete GreyTexture;
	delete AsteroidBlasterMesh;
	delete BlasterMarkingsMesh;
	delete AmmoMesh;
	delete AmmoCoreMesh;

	delete AsteroidBaseMesh;
}

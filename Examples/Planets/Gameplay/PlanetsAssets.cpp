
#include "PlanetsAssets.h"
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>

using namespace igad;

PlanetsAssets::PlanetsAssets(World& world) : Component(world)
{
	BasicShader = new Shader("./Assets/Basic.vsh",
								"./Assets/Basic.fsh");
	WhiteTexture = new Texture("./Assets/Textures/white.png");
	GreyTexture = new Texture("./Assets/Textures/darkgrey.png");

	GeoSpheres[0] = new Mesh("./Assets/Models/GeoSphere8.obj");

	GeoSpheres[1] = new Mesh("./Assets/Models/GeoSphere16.obj");

	GeoSpheres[2] = new Mesh("./Assets/Models/GeoSphere20.obj");

	GeoSpheres[3] = new Mesh("./Assets/Models/GeoSphere32.obj");

	GeoSpheres[4] = new Mesh("./Assets/Models/GeoSphere64.obj");

	GeoSpheres[5] = new Mesh("./Assets/Models/GeoSphere80.obj");
}

PlanetsAssets::~PlanetsAssets()
{
}

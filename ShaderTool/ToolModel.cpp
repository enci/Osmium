#include "ToolModel.h"
#include <Core/Game.h>
#include <Core/Resources.h>
#include <Graphics/Mesh.h>
#include <Graphics/Shader.h>
#include <Graphics/Texture.h>
#include <Tools/MeshGenerators.h>
#include "imgui/imgui.h"

using namespace Osm;

ToolModel::ToolModel(Osm::World& world) : Entity(world)
{
	SetName("Model");

	//auto mesh = Game.Resources().LoadResource<Mesh>("./Assets/Models/SmoothSphere.obj");

	auto shader = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Planet.vsh",
		"./Assets/Shaders/Planet.gsh",
		"./Assets/Shaders/Planet.fsh");
	auto texture = Game.Resources().LoadResource<Texture>("./Assets/Textures/white.png");

	_transform = CreateComponent<Transform>();
	_renderer = CreateComponent<MeshRenderer>();
	_renderer->SetTexture(texture);
	_renderer->SetShader(shader);
	Setup();
}

void ToolModel::Update(float dt)
{
	if(_dirty)
	{
		Setup();
	}
	_dirty = false;
}

void ToolModel::Inspect()
{
	int* m = (int*)&_model;
	const char* model[] = { "Sphere", "Plane", "Teapot" };
	if(ImGui::Combo("Model", m, model, 3))
	{
		_dirty = true;
	}
	switch (_model)
	{
	case Sphere:
		if(ImGui::InputInt("Sphere Tesselation", &_sphereTesselation))
			_dirty = true;
		break;
	case Plane:
		if(ImGui::InputInt("Plane Resolution", &_planeResolution))
			_dirty = true;
		break;
	case Teappot:
		break;
	default:;
	}

	
	Entity::Inspect();
}

void ToolModel::Setup()
{
	if (_mesh && _mesh == _teapot)
		Game.Resources().ReleaseResource(_mesh);
	else
		delete _mesh;

	_mesh = new Mesh();

	vector<VertexFormat> vertices;
	vector<ushort> indices;

	switch (_model)
	{
	case Sphere:
		GenerateIcosphere(_size, _sphereTesselation, vertices, indices);;
		break;
	case Plane:
		GeneratePlane(10.0f * _size, _planeResolution, vertices, indices);;
		break;
	case Teappot:
		break;
	default:;
	}

	_mesh->SetVertices(move(vertices));
	_mesh->SetIndices(move(indices));
	_mesh->Apply();
	_renderer->SetMesh(_mesh);
}

#include "ToolWorld.h"
#include <Graphics/DebugRenderer.h>
#include <Graphics/Mesh.h>
#include <Core/Game.h>
#include <Core/Resources.h>
#include <Graphics/Render.h>
#include <Graphics/MeshRenderer.h>
#include <Graphics/Texture.h>

using namespace Osm;

ToolWorld::ToolWorld()
{
	_renderManager = CreateComponent<Osm::RenderManager>();
	_camera = CreateEntity<ToolCamera>();
	gDebugRenderer.Initialize();


	auto mesh = Game.Resources().LoadResource<Mesh>("./Assets/Models/SmoothSphere.obj");
	auto shader = Game.Resources().LoadResource<Shader>(
		"./Assets/Shaders/Planet.vsh",
		"./Assets/Shaders/Planet.fsh");
	auto texture = Game.Resources().LoadResource<Texture>("./Assets/Textures/white.png");
	auto sphereEnt = CreateEntity<Entity>();
	sphereEnt->SetName("Smooth Sphere");
	auto trn = sphereEnt->CreateComponent<Transform>();
	auto renderer = sphereEnt->CreateComponent<MeshRenderer>();
	renderer->SetMesh(mesh);
	renderer->SetTexture(texture);
	//renderer->SetDiffuse(Color("95B9C900"));
	//renderer->SetAmbient(Color("07242A00"));
	renderer->SetShader(shader);

	// White light
	auto whiteLight = CreateEntity<Entity>();
	auto transform = whiteLight->CreateComponent<Transform>();
	auto light = whiteLight->CreateComponent<Light>();
	light->SetColor(Color::White);
	transform->SetOrientation(Matrix44::CreateRotateX(DegToRad(-20.0)) * Matrix44::CreateRotateY(DegToRad(-20.0)));
}

void ToolWorld::Render()
{
	Color bg = _camera->GetComponent<Camera>()->GetClearColor();
	glClearColor(bg.r / 255.0f, bg.g / 255.0f, bg.b / 255.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Turn on wireframe mode
	// glPolygonMode(GL_FRONT, GL_LINE);
	// glPolygonMode(GL_BACK, GL_LINE);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	_renderManager->Render();

	/*
	if (_debugRender)
	{
		Camera* camera = _camera->GetComponent<Camera>();
		Matrix44 vp = camera->Projection() * camera->GetView();
		gDebugRenderer.Draw(vp);
	}
	*/
}

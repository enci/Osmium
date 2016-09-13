#include "PlanetWorld.h"
#include <Graphics/Renderer.h>
#include <Core/Device.h>
#include "PlanetsAssets.h"
#include <ctime>

using namespace igad;


Celesital::Celesital(igad::World& world) : Entity(world)
{
	rot = RandInRange(-Pi, Pi);
}

void Celesital::Update(float dt)
{
	auto trns = GetComponent<Transform>();
	if (dist == -1.0f)
	{
		dist = trns->GetPosition().Magnitude();
		vel = 5.0f / dist;
	}

	rot += vel * dt;

	Vector3 pos = Vector3(dist * cos(rot), 0.0f, dist * sin(rot));
	trns->SetPosition(pos);
}

PlanetWorld::PlanetWorld()
{
	srand(time(NULL));

	auto ren = CreateComponent<Renderer>();	
	auto ast = CreateComponent<PlanetsAssets>();
	ren->Init(ast->BasicShader);

	float ratio = (float)pDevice->GetScreenWidth() / pDevice->GetScreenHeight();
	// 
	_view = Matrix44::CreateLookAt(Vector3(50.0f, 25.0f, 50.0f), Vector3(), Vector3(0, 1, 0));
	_projection = Matrix44::CreatePerspective(DegToRad(15.0f), ratio, 3.0f, 1000.0f);

	// Create sun
	auto sun = CreateEntity<Entity>();
	auto sunTrns = sun->CreateComponent<Transform>();
	auto sunRen = sun->CreateComponent<MeshRenderer>();
	sunRen->Init(ast->GeoSpheres[5], ast->WhiteTexture);

	for(int i = 0; i < 8; i++)
	{

		auto parent = CreateEntity<Celesital>();
		auto parentTrns = parent->CreateComponent<Transform>();
		parentTrns->SetPosition(
			Vector3(RandInRange(-15.0f, 15.0f),
					0,
					RandInRange(-15.0f, 15.0f)));

		auto planet = CreateEntity<Entity>();
		auto planetTrns = planet->CreateComponent<Transform>();
		auto planetRen = planet->CreateComponent<MeshRenderer>();
		planetRen->Init(ast->GeoSpheres[4], ast->GreyTexture);		
		float s = RandInRange(0.1f, 0.4f);
		planetTrns->SetScale(Vector3(s, s, s));
		planetTrns->SetParent(parentTrns);

		while(RandInRange(0.0f, 1.0f) < 0.2f)
		{
			auto moon =	
		}
	}
}

float f = 0;

void PlanetWorld::Render()
{
	auto ren = GetComponent<Renderer>();
	auto ast = GetComponent<PlanetsAssets>();

	ren->Begin(_view, _projection);

	for (auto& e : _entities)
	{

		auto r = e->GetComponent<MeshRenderer>();
		if (r)
		{
			auto t = e->GetComponent<Transform>();
			ASSERT(t);
			ren->Render(t->GetWorld(), r->GetTexture(), r->GetMesh());
		}
	}

	ren->End();
}


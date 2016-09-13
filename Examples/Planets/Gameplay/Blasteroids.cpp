
#include "Blasteroids.h"
#include <Graphics/DebugRenderer.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <Core/Device.h>
#include "Spaceship.h"
#include "Asteroid.h"
#include "BlasteroidsCamera.h"
#include "BlasteroidsAssets.h"
#include "Ammo.h"
#include "Crate.h"
#include "Crystal.h"


using namespace igad;
using namespace std;

int Blasteroids::_players = 4;
vector<int> Blasteroids::_score = {0, 0, 0, 0};
int Blasteroids::_count = 0;

void Blasteroids::CreateAsteroidsLevel()
{
	_background = Color("000a38");
	for (size_t i = 0; i < 17; i++)
	{
		Vector3 pos(RandInRange(-WorldWidth, WorldWidth),
		            0.0f,
		            RandInRange(-WorldHeight, WorldHeight));
		Vector2 vel(RandInRange(-1, 1),
		            RandInRange(-1, 1));
		float r = RandInRange(5.0f, 20.0f);
		auto a = CreateEntity<Asteroid>();
		a->Init(pos, vel * 3.0f, r);
	}	

	float ex = 3.3f;
	for (size_t i = 0; i < 120; i++)
	{
		Vector3 pos(RandInRange(-WorldWidth * ex, WorldWidth * ex),
		            RandInRange(-WorldDepth, -35.0f),
		            RandInRange(-WorldHeight * ex, WorldHeight * ex));
		float r = RandInRange(7.0f, 32.0f);
		auto a = CreateEntity<Asteroid>();
		a->Init(pos, Vector2(), r);
	}

	CrateDust();
	CreateStars();

	Transform*	transform;
	Light*		light;

	// White light
	auto whiteLight = CreateEntity<Entity>();
	transform = whiteLight->CreateComponent<Transform>();
	light = whiteLight->CreateComponent<Light>();
	light->SetColor(Color::White * 0.65f);
	auto& wmtx = transform->GetTransform();
	wmtx = wmtx * Matrix44::CreateRotateX(DegToRad(-20.0)) * Matrix44::CreateRotateY(DegToRad(-20.0));

	// Blue light
	auto blueLight = CreateEntity<Entity>();
	transform = blueLight->CreateComponent<Transform>();
	light = blueLight->CreateComponent<Light>();
	light->SetColor(Color::Blue * 0.30f);
	auto& bmtx = transform->GetTransform();
	bmtx = bmtx * Matrix44::CreateRotateX(DegToRad(-20.0)) * Matrix44::CreateRotateY(DegToRad(-180.0));

	auto planet = CreateEntity<Entity>();
	transform = planet->CreateComponent<Transform>();
	auto planetMesh = planet->CreateComponent<MeshRenderer>();
	planetMesh->SetMesh(_assets->PlanetMesh);
	planetMesh->SetTexture(_assets->WhiteTexture);
	planetMesh->SetShader(_assets->AsteroidShader);
	transform->SetPosition(Vector3(-600.0f, -1020.0f, 200.0f));
	transform->SetUniformScale(700.0f);
}

void Blasteroids::CreateCrystalsLevel()
{
	_background = Color("d0d8e3");
	for (size_t i = 0; i < 10; i++)
	{
		Vector3 pos(RandInRange(-WorldWidth, WorldWidth),
			0.0f,
			RandInRange(-WorldHeight, WorldHeight));
		Vector2 vel(RandInRange(-1, 1),
			RandInRange(-1, 1));
		float r = RandInRange(5.0f, 20.0f);
		auto a = CreateEntity<Crystal>();
		a->Init(pos, vel * 3.0f, r);
	}

	/*
	float ex = 3.3f;
	for (size_t i = 0; i < 20; i++)
	{
		Vector3 pos(RandInRange(-WorldWidth * ex, WorldWidth * ex),
			RandInRange(-WorldDepth, -35.0f),
			RandInRange(-WorldHeight * ex, WorldHeight * ex));
		float r = RandInRange(7.0f, 32.0f);
		auto a = CreateEntity<Asteroid>();
		a->Init(pos, Vector2(), r);
	}

	*/

	CrateDust();
	CreateStars();

	Transform*	transform;
	Light*		light;

	// White light
	auto whiteLight = CreateEntity<Entity>();
	transform = whiteLight->CreateComponent<Transform>();
	light = whiteLight->CreateComponent<Light>();
	light->SetColor(Color::White);
	auto& wmtx = transform->GetTransform();
	wmtx = wmtx * Matrix44::CreateRotateX(DegToRad(-20.0)) * Matrix44::CreateRotateY(DegToRad(-20.0));
}

void Blasteroids::CreateOrangeLevel()
{
	_background = Color("773618");

	const int n = 7;
	const int m = 4;
	const float speed = 0.0f;
	const float w = 240;
	const float h = 160;
	const float dx = w / n;
	const float dy = h / m;
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < m; j++)
		{
			Vector3 pos((w - dx) * -0.5f + i * dx, 0.0f, (h - dy) * -0.5f + j * dy);
			Vector2 vel(RandInRange(-speed, speed), RandInRange(-speed, speed));
			float r = RandInRange(5.0f, 20.0f);
			auto a = CreateEntity<Crate>();
			a->Init(pos, vel);
		}
	}

	/*
	float ex = 3.3f;
	for (size_t i = 0; i < 120; i++)
	{
		Vector3 pos(RandInRange(-WorldWidth * ex, WorldWidth * ex),
			RandInRange(-WorldDepth, -35.0f),
			RandInRange(-WorldHeight * ex, WorldHeight * ex));
		float r = RandInRange(7.0f, 32.0f);
		auto a = CreateEntity<Asteroid>();
		a->Init(pos, Vector2(), r);
	}
	*/

	CrateDust();
	// CreateStars();

	Transform*	transform;
	Light*		light;

	// White light
	auto whiteLight = CreateEntity<Entity>();
	transform = whiteLight->CreateComponent<Transform>();
	light = whiteLight->CreateComponent<Light>();
	light->SetColor(Color::Orange * 0.85f);
	auto& wmtx = transform->GetTransform();
	wmtx = wmtx * Matrix44::CreateRotateX(DegToRad(-20.0)) * Matrix44::CreateRotateY(DegToRad(-20.0));

	/*
	// Blue light
	auto blueLight = CreateEntity<Entity>();
	transform = blueLight->CreateComponent<Transform>();
	light = blueLight->CreateComponent<Light>();
	light->SetColor(Color::Blue * 0.30f);
	auto& bmtx = transform->GetTransform();
	bmtx = bmtx * Matrix44::CreateRotateX(DegToRad(-20.0)) * Matrix44::CreateRotateY(DegToRad(-180.0));
	*/
}

Blasteroids::Blasteroids()
	: WorldWidth(140.0f)
	, WorldHeight(80.0f)
	, WorldDepth(380.0f)
{
	srand(time(nullptr));
	_assets = CreateComponent<BlasteroidsAssets>();
	_renderManager = CreateComponent<RenderManager>();
	_physicsManager2D = CreateComponent<PhysicsManager2D>();

	_camera = CreateEntity<BlasteroidsCamera>();

	auto particleSysEntity = CreateEntity<Entity>();
	_particleSystem = particleSysEntity->CreateComponent<ParticleSystem>();
	_particleSystem->Init(5000);
	_particleSystem->SetShader(_assets->ParticleShader);


	int player = 0;
	for (int joy = 0; joy < 6 && player < 4; joy++)
	{
		if (glfwJoystickPresent(joy))
		{
			SpaceShip* ship = CreateEntity<SpaceShip>();
			ship->Init(player, joy);
			player++;
		}
	}
	
	while(player < 2)
	{
		SpaceShip* ship = CreateEntity<SpaceShip>();
		ship->Init(player++, -1);
	}

	CreateAmmo();

	_count++;
	_count %= 5;
	switch (_count)
	{
	case 4:
		CreateOrangeLevel();
		break;
	case 3:
		CreateCrystalsLevel();
		break;
	default:
		CreateAsteroidsLevel();
		break;
	}

	gDebugRenderer.Initialize();
}

void Blasteroids::Update(float dt)
{
	World::Update(dt);	// Update all the entities

	UpdateRenderFlags();
	_physicsManager2D->UpdatePhysics(dt);
	_particleSystem->Update(dt);

	World::PostUpdate(dt);
}

void Blasteroids::Render()
{
	Color bg = _background;
	glClearColor(bg.r / 255.0f, bg.g / 255.0f, bg.b / 255.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	if (_renderScene)
		_renderManager->Render();

	if (_debugRender)
	{
		Camera* camera = _camera->GetComponent<Camera>();
		Matrix44 vp = camera->Projection() * camera->GetView();
		gDebugRenderer.Draw(vp);
		gDebugRenderer.Clear();
	}
}

ParticleSystem* Blasteroids::GetParticles()
{
	return _particleSystem;
}

bool Blasteroids::IsDone()
{
	if (_isover)
		return true;

	auto ships = GetEntitiesByType<SpaceShip>();
	if (ships.size() <= 1)
	{
		_score[ships[0]->GetPlayer()]++;
		_isover = true;
	}

	return false;
}

void Blasteroids::CrateDust()
{
	auto dustEntity = CreateEntity<Entity>();
	auto dust = dustEntity->CreateComponent<ParticleSystem>();
	auto assets = GetComponent<BlasteroidsAssets>();

	dust->Init(500);
	dust->SetShader(assets->DustShader);
	float ex = 2.8f;
	for (size_t i = 0; i < 500; i++)
	{
		Vector3 pos(RandInRange(-WorldWidth * ex, WorldWidth * ex),
					RandInRange(-WorldDepth, 15.0f),
					RandInRange(-WorldHeight * ex, WorldHeight * ex));
		float size = RandInRange(0.15f, 0.5f);
		Color color("c9b284");
		color = color * RandInRange(0.8f, 1.0f);
		dust->AddParticle(pos, Vector3(), Vector3(), 0.0f, color, color, size, size, FLT_MAX, 1.0f);
	}
	dust->Update(0.0f);
}

void Blasteroids::CreateStars()
{
	auto starsEntity = CreateEntity<Entity>();
	auto stars = starsEntity->CreateComponent<ParticleSystem>();
	auto assets = GetComponent<BlasteroidsAssets>();

	stars->Init(500);
	stars->SetShader(assets->DustShader);
	float ex = 1800.0f / 100.0f;
	for (size_t i = 0; i < 500; i++)
	{
		Vector3 pos(RandInRange(-WorldWidth * ex, WorldWidth * ex),
					RandInRange(-1800.0f, -1400.0f),
					RandInRange(-WorldHeight * ex, WorldHeight * ex));
		float size = RandInRange(1.15f, 3.5f);
		Color color("e8d8cb");
		color = color * RandInRange(0.8f, 1.0f);
		stars->AddParticle(pos, Vector3(), Vector3(), 0.0f, color, color, size, size, FLT_MAX, 1.0f);
	}
	stars->Update(0.0f);
}

void Blasteroids::UpdateRenderFlags()
{
	int shiftState = glfwGetKey(pDevice->GetWindow(), GLFW_KEY_RIGHT_SHIFT);
	bool rState = pDevice->GetKeyOnce(GLFW_KEY_Z);
	bool dbgState = pDevice->GetKeyOnce(GLFW_KEY_X);

	if (shiftState == GLFW_PRESS && rState)
		_renderScene = !_renderScene;

	if (shiftState == GLFW_PRESS && dbgState)
		_debugRender = !_debugRender;
}

void Blasteroids::CreateAmmo()
{
	auto ammo = CreateEntity<Ammo>();
}

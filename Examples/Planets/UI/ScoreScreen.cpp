
#include "ScoreScreen.h"
#include "../Gameplay/BlasteroidsAssets.h"
#include "../Gameplay/BlasteroidsCamera.h"
#include <Core/Device.h>
#include "../Gameplay/Blasteroids.h"

using namespace igad;

SpaceshipIcon::SpaceshipIcon(igad::World& world) : Entity(world)
{
	_transform = CreateComponent<Transform>();

	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();
	_renderer = CreateComponent<MeshRenderer>();
	_renderer->SetMesh(assets->AsteroidBlasterMesh);
	_renderer->SetShader(assets->AsteroidShader);
	_renderer->SetTexture(assets->WhiteTexture);

	_markingsRenderer = CreateComponent<MeshRenderer>();
	_markingsRenderer->SetMesh(assets->BlasterMarkingsMesh);
	_markingsRenderer->SetShader(assets->AsteroidShader);
	_markingsRenderer->SetTexture(assets->WhiteTexture);

	//_particleSystem = _blasteroidsWorld->GetParticles();
}

SpaceshipIcon::~SpaceshipIcon()
{
}

void SpaceshipIcon::Init(int player, int joystick)
{
	_player = player;
	_joystick = joystick;

	_transform->SetPosition(Vector3(80.0f, 0.0f, -20 + player * 20));

	_color = dynamic_cast<ScoreScreen&>(GetWorld()).PlayerColors[_player];

	_markingsRenderer->SetDiffuse(_color);
	_markingsRenderer->SetAmbient(_color);
}

void SpaceshipIcon::Update(float dt)
{
}

ScoreScreen::ScoreScreen()
{
	_timer = 0.0f;

	_assets = CreateComponent<BlasteroidsAssets>();
	_renderManager = CreateComponent<RenderManager>();
	_physicsManager2D = CreateComponent<PhysicsManager2D>();

	auto particleSysEntity = CreateEntity<Entity>();
	_particleSystem = particleSysEntity->CreateComponent<ParticleSystem>();
	_particleSystem->Init(1000);
	_particleSystem->SetShader(_assets->ParticleShader);

	Transform*	transform;
	auto cameraEntity = CreateEntity<Entity>();
	transform = cameraEntity->CreateComponent<Transform>();
	auto camera = cameraEntity->CreateComponent<Camera>();
	float fov = 60.0f;
	float dist = 100.0f;
	Vector3 position = Vector3(0, dist, 0);
	int screenWidth = pDevice->GetScreenWidth();
	int screenHeight = pDevice->GetScreenHeight();
	float ratio = float(screenWidth) / float(screenHeight);
	Matrix44 view = Matrix44::CreateLookAt(position, Vector3(), Vector3(0, 0, 1));
	Matrix44 projection = Matrix44::CreatePerspective(DegToRad(fov), ratio, 0.1f, 2000.0f);
	camera->SetView(view);
	camera->SetProjection(projection);

	for (int player = 0; player < 4; player++)
	{
		auto ship = CreateEntity<SpaceshipIcon>();
		ship->Init(player, 0);

		auto shipTransform = ship->GetComponent<Transform>();

		for(int i = 0; i < Blasteroids::_score[player]; i++)
		{
			auto scoreBox = CreateEntity<Entity>();
			auto scoreTransform = scoreBox->CreateComponent<Transform>();
			scoreTransform->SetPosition(shipTransform->GetPosition() + Vector3(- 15 - (6 * i), 0, 0));
			scoreTransform->SetScale(Vector3(1, 1, 6));
			auto pushMesh = scoreBox->CreateComponent<MeshRenderer>();
			pushMesh->SetMesh(_assets->BoxMesh);
			pushMesh->SetTexture(_assets->WhiteTexture);
			pushMesh->SetShader(_assets->AsteroidShader);
			pushMesh->SetAmbient(PlayerColors[player]);
		}
	}

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
}

void ScoreScreen::Update(float dt)
{
	_timer += dt;
}

void ScoreScreen::Render()
{
	Color bg = Color::Black;
	glClearColor(bg.r / 255.0f, bg.g / 255.0f, bg.b / 255.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_renderManager->Render();
}

bool ScoreScreen::IsDone()
{
	if (_timer > 0.0f)
		return true;
	return false;
}

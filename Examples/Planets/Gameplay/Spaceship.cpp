#include "Spaceship.h"
#include <Core/World.h>
#include <Core/Transform.h>
#include <Graphics/DebugRenderer.h>
#include <Graphics/Render.h>
#include <GLFW/glfw3.h>
#include <Core/Device.h>
#include <Utils.h>
#include "BlasteroidsAssets.h"
#include "Projectiles.h"
#include "Wrapper.h"
#include "Blasteroids.h"

using namespace igad;

const float kDensity = 2.0f;
const float kRadius = 3.0f;
const float kThrustMultiplier = 5000.0f;
const float kTurnForce = 285000.0f;
const float kPushReloadTime = 6.0f;

InfoBar::InfoBar(igad::World& world) : Entity(world)
{
	_transform = CreateComponent<Transform>();

	_mesh = new Mesh();
	//_mesh->Apply();

	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();
	_renderer = CreateComponent<MeshRenderer>();
}

InfoBar::~InfoBar()
{
}

void InfoBar::Init(int player)
{
	_color = dynamic_cast<Blasteroids*>(&GetWorld())->PlayerColors[player];

	_renderer->SetDiffuse(_color);
	_renderer->SetAmbient(_color);
}

void InfoBar::UpdateInfo(float health, float push, float ammo, bool special)
{
	static int n = 0;
	
	if (push > 1.0f) push = 1.0f;
	health /= 100.0f;

	vector<VertexFormat> vertices;
	vertices.reserve(24);
	
	const float sz = 3.0f;	// Size of bottom bar
	const float sp = 0.6f;
	const float h = sz * sqrt(3.0f) * 0.5f;
	const float l = 15;

	// Push
	size_t pushBase = vertices.size();
	Vector3 pushColor(1, 1, 1);
	Vector2 uv(0,0);
	vertices.push_back({ Vector3(0.0f, 0.0f, 0.0f), pushColor, uv });
	vertices.push_back({ Vector3(-sz * 0.5f, 0.0f, -h), pushColor, uv });
	vertices.push_back({ Vector3(-sz, 0.0f, 0.0f), pushColor, uv });

	Vector3 cen = (	vertices[pushBase + 0].Position +
					vertices[pushBase + 1].Position +
					vertices[pushBase + 2].Position) / 3.0f;
	float val = push * 0.8f + 0.2f;
	vertices.push_back({ Lerp(vertices[pushBase + 0].Position, cen, val), pushColor, uv });
	vertices.push_back({ Lerp(vertices[pushBase + 1].Position, cen, val), pushColor, uv });
	vertices.push_back({ Lerp(vertices[pushBase + 2].Position, cen, val), pushColor, uv });

	// vertices[pushBase + 3].Position = Vector3(0.0f, 0.0f, 0.0f);
	// vertices[pushBase + 4].Position = Vector3(-sz * 0.5f, 0.0f, -h);
	// vertices[2].Position = Vector3(-sz, 0.0f, 0.0f);

	// Health
	const float hl = l - sz - sp;
	size_t healthIdx = vertices.size();
	Vector3 healthColor(1, 0, 1);
	vertices.push_back({ vertices[pushBase + 2].Position - Vector3(sp, 0.0f, 0.0f), healthColor, uv });
	vertices.push_back({ vertices[pushBase + 1].Position - Vector3(sp, 0.0f, 0.0f), healthColor, uv });
	vertices.push_back({ vertices[healthIdx + 0].Position - Vector3(hl * health, 0.0f, 0.0f), healthColor, uv });
	vertices.push_back({ vertices[healthIdx + 1].Position - Vector3(hl * health, 0.0f, 0.0f), healthColor, uv });

	
	const float szm = sz * 0.5f;
	const float hm = szm * sqrt(3.0f) * 0.5f;
	size_t specIdx = vertices.size();

	// Special ammo
	vertices.push_back({ Vector3(0.0f, 0.0f, sp), healthColor, uv });
	vertices.push_back({ Vector3(-szm * 0.5f, 0.0f, sp + hm), healthColor, uv });
	vertices.push_back({ Vector3(-szm, 0.0f, sp), healthColor, uv });

	// Ammo 
	const float al = l - szm - sp;
	ammo /= 10;
	size_t ammoIdx = vertices.size();
	vertices.push_back({ vertices[specIdx + 1].Position - Vector3(sp, 0.0f, 0.0f), healthColor, uv });
	vertices.push_back({ vertices[specIdx + 2].Position - Vector3(sp, 0.0f, 0.0f), healthColor, uv });
	vertices.push_back({ vertices[ammoIdx + 0].Position - Vector3(al * ammo, 0.0f, 0.0f), healthColor, uv });
	vertices.push_back({ vertices[ammoIdx + 1].Position - Vector3(al * ammo, 0.0f, 0.0f), healthColor, uv });

	/*
	for (auto& v : vertices)
	{
		v.Normal = Vector3(0, 1, 0);
		v.Texture = Vector2(0, 0);
	}

	if (push >= 0.0f)
		vertices[0].Position = vertices[1].Position = vertices[2].Position = Vector3(0, 0, 0);

	//vector<ushort> indices = {	0, 1, 2, 3, 4, 5, 4, 6, 5,
	//							7, 9, 8, 10, 11, 12, 11, 13, 12 };

	*/

	//vector<ushort> indices = { ushort(pushBase + 0), ushort(pushBase + 1), ushort(pushBase + 2) };

	vector<ushort> pushIndices = {	0, 1, 3,
									3, 1, 4,
									1, 2, 4,
									4, 2, 5,
									2, 0, 5,
									5, 0, 3 };
	for (auto& idx : pushIndices)
		idx = ushort(pushBase + idx);

	vector<ushort> healthIndices = { 0, 1, 2, 1, 3, 2 };
	for (auto& idx : healthIndices)
		idx = ushort(healthIdx + idx);

	vector<ushort> specIndices = { 0, 2, 1 };
	for (auto& idx : specIndices)
		idx = ushort(specIdx + idx);

	vector<ushort> ammoIndices = { 0, 1, 2, 1, 3, 2 };
	for (auto& idx : ammoIndices)
		idx = ushort(ammoIdx + idx);

	vector<ushort> indices;
	indices.insert(indices.end(), pushIndices.begin(), pushIndices.end());
	indices.insert(indices.end(), healthIndices.begin(), healthIndices.end());
	indices.insert(indices.end(), specIndices.begin(), specIndices.end());
	indices.insert(indices.end(), ammoIndices.begin(), ammoIndices.end());

	_mesh->SetIndices(move(indices));
	_mesh->SetVertices(move(vertices));
	_mesh->Apply();
	_renderer->SetMesh(_mesh);
	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();
	_renderer->SetShader(assets->AsteroidShader);
	_renderer->SetTexture(assets->WhiteTexture);
}


SpaceShip::SpaceShip(World& world) : Entity(world)
{
	_blasteroidsWorld = dynamic_cast<Blasteroids*>(&GetWorld());
	_transform = CreateComponent<Transform>();

	_light = CreateComponent<Light>();
	_light->SetLightType(Light::POINT_LIGHT);

	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();
	_renderer = CreateComponent<MeshRenderer>();
	_renderer->SetMesh(assets->AsteroidBlasterMesh);
	_renderer->SetShader(assets->AsteroidShader);
	_renderer->SetTexture(assets->WhiteTexture);

	_markingsRenderer = CreateComponent<MeshRenderer>();
	_markingsRenderer->SetMesh(assets->BlasterMarkingsMesh);
	_markingsRenderer->SetShader(assets->AsteroidShader);
	_markingsRenderer->SetTexture(assets->WhiteTexture);

	_physicsBody = CreateComponent<PhysicsBody2D>();
	_physicsBody->SetLinearDamping(1.0f);
	_physicsBody->SetRadius(3.0f);
	_physicsBody->SetMass(DiskArea(kRadius) * kDensity);
	vector<Vector2> collision =
	{
		Vector2(-3, -2),
		Vector2(-2, 8),
		Vector2(2, 8),
		Vector2(3, -2)
	};
	_physicsBody->SetCollisionShape(move(collision));

	_wrapper = CreateComponent<Wrapper>();
	
	_particleSystem = _blasteroidsWorld->GetParticles();
}

SpaceShip::~SpaceShip()
{
}

void SpaceShip::Init(int player, int joystick)
{
	_player = player;
	_joystick = joystick;
	SetButtonMapping();

	_color = _blasteroidsWorld->PlayerColors[_player];
	_light->SetColor(_color);

	float xPos = -20.0f;
	float yPos = 20.0f;
	if (player % 2)
	{
		xPos *= -1;
		yPos *= -1;
	}
	if (player > 1)
		yPos *= -1;

	Vector3 pos(xPos, 0.0f, yPos);
	_transform->SetPosition(pos);
	_physicsBody->SetOrientation(atan2(-yPos, -xPos));

	_markingsRenderer->SetDiffuse(_color);
	_markingsRenderer->SetAmbient(_color);

	_infoBar = GetWorld().CreateEntity<InfoBar>();
	_infoBar->Init(_player);
	_infoBar->UpdateInfo(_health, _pushTimer, _ammo, true);
}

void SpaceShip::Update(float dt)
{
	Vector2 direction;
	float thrust;
	bool shoot;
	bool push;

	GetInput(direction, thrust, shoot, push);	

	Matrix44& transform = _transform->GetTransform();
	Vector2 fwd = ToVector2(transform.GetZAxis());

	GenerateTurningTorque(direction, fwd);
	
	Vector2 force = fwd * thrust * kThrustMultiplier;
	_physicsBody->AddForce(force);

	if (shoot && !_shot && _ammo > 0)
	{
		_shot = true;
		auto projectile = GetWorld().CreateEntity<Projectile>();
		projectile->Init(this);
		_physicsBody->AddForce(-4000.0f * fwd);
		_ammo--;
	}
	else if (!shoot)
	{
		_shot = false;
	}

	_pushTimer += dt;
	if (push && _pushTimer >= kPushReloadTime)
	{
		auto p = GetWorld().CreateEntity<Push>();
		p->Init(this);
		_physicsBody->AddForce(-180000.0f * fwd);
		_pushTimer = 0.0f;
	}

	_wrapper->Update(dt);

	Color lightColor = _color * 0.3f;
	int partPerSec = -thrust * _particlesPerSecond + 2;
	float timePerParticle = 1.0f / partPerSec;
	_particleTime += dt;
	if (_particleTime > timePerParticle)
	{
		Vector3 rnd = RandomOnUnitSphere();
		float size = RandInRange(1.5f, 3.0f);
		float time = RandInRange(2.0f, 3.5f);

		Vector3 axis = RandomOnUnitSphere();
		axis.Normalize();
		float rotSpeed = RandInRange(-1.0f, 1.0f);
		
		Color color = _color;
		color.a *= 0.85f;
		_particleSystem->AddParticle(	_transform->GetPosition(), -ToVector3(force) * 0.002f + rnd * 0.3f,
										axis, rotSpeed, color, color, size, 0.0f, time);
		_particleTime = 0.0f;

		lightColor = _color * (0.3f + thrust * 0.7f);
	}

	_light->SetColor(lightColor);

	_particleSystem->Update(dt);

	_infoBar->UpdateInfo(_health, _pushTimer / kPushReloadTime, _ammo, true);
	auto infoTransform = _infoBar->GetComponent<Transform>();
	infoTransform->SetPosition(_transform->GetPosition() + Vector3(0.0f, 12.0f, 10.0f));

	if(_health <= 0)
	{
		for (size_t i = 0; i < 40; i++)
		{
			Vector3 rnd = RandomOnUnitSphere();
			rnd.y *= 0.2f;
			float size = RandInRange(1.8, 7.0f);
			float time = RandInRange(2.0f, 3.5f);
			float speed = RandInRange(20.0f, 50.0f);

			Vector3 axis = RandomOnUnitSphere();
			axis.Normalize();
			float rotSpeed = RandInRange(-1.0f, 1.0f);

			Vector3 pos = _transform->GetPosition();
			Color color = _color;
			color.a = 180;

			_particleSystem->AddParticle(pos, rnd * speed,
				axis, rotSpeed, color, color, size, 0.0f, time);
		}

		GetWorld().RemoveEntity(_infoBar);
		GetWorld().RemoveEntity(this);
	}
}

void SpaceShip::PostUpdate(float dt)
{
	return;
	//  _physicsBody->GetAngularVelocity();
	float rot = 0.2f;
	auto& transform = _transform->GetTransform();
	transform = Matrix44::CreateRotate(rot, transform.GetXAxis()) * transform;
}

void SpaceShip::OnCollision(Collision2D& collision, int body)
{
}


void SpaceShip::GenerateTurningTorque(Vector2 direction, Vector2 forward)
{	
	float k = kTurnForce;
	float c = k / 4.0f;
	if (direction.Magnitude() > 0.3f)
	{
		direction.Normalize();
		forward.Normalize();

		Vector2 perp = direction.Perpendicular();
		float dot = direction.Dot(forward);
		if (dot > 1.0f) dot = 1.0f;
		if (dot < 0.0f) dot = 0.0f;		

		float angle = -acos(dot);
		if (perp.Dot(forward) < 0)
			angle = -angle;

		float torque = angle * k - c * _physicsBody->GetAngularVelocity();
		_physicsBody->AddTorque(torque);
	}
}

void SpaceShip::SetButtonMapping()
{
	switch (_player)
	{
	case 0:
		_upButton = GLFW_KEY_UP;
		_downButton = GLFW_KEY_DOWN;
		_leftButon = GLFW_KEY_LEFT;
		_rightButton = GLFW_KEY_RIGHT;
		_shootButton = GLFW_KEY_M;
		_thrustButton = GLFW_KEY_N;
		break;
	case 1:
		_upButton = GLFW_KEY_W;
		_downButton = GLFW_KEY_S;
		_leftButon = GLFW_KEY_A;
		_rightButton = GLFW_KEY_D;
		_shootButton = GLFW_KEY_R;
		_thrustButton = GLFW_KEY_T;
		break;
	default:
		break;
	}
}

void SpaceShip::GetInput(Vector2& direction, float& thrust, bool& shoot, bool& push)
{
	direction.Clear();
	thrust = 0.0f;
	shoot = false;
	push = false;

	if(_joystick == -1)
	{
		if (glfwGetKey(pDevice->GetWindow(), _leftButon))
			direction.x += 1.0;
		if (glfwGetKey(pDevice->GetWindow(), _rightButton))
			direction.x -= 1.0;
		if (glfwGetKey(pDevice->GetWindow(), _upButton))
			direction.y += 1.0;
		if (glfwGetKey(pDevice->GetWindow(), _downButton))
			direction.y -= 1.0;
		if (glfwGetKey(pDevice->GetWindow(), _thrustButton))
			thrust = 1.0f;
		if (glfwGetKey(pDevice->GetWindow(), _shootButton))
			shoot = true;

		//thrust += 0.06f;
		//_health = 100;
	}
	else
	{
		int p = glfwJoystickPresent(_joystick);
		if (p)
		{
			int count;
			const float* axes = glfwGetJoystickAxes(_joystick, &count);
			if (count >= 3)
			{
				direction.x = -axes[0];
				direction.y = -axes[1]; 
				thrust = -axes[2];
			}

			const unsigned char* buttons = glfwGetJoystickButtons(_joystick, &count);
			if (count > 3)
			{
				shoot = buttons[0];
				push = buttons[1];
			}
		}		
	}

	if (direction.SquareMagnitude() > 0.1f)
		direction.Normalize();
}


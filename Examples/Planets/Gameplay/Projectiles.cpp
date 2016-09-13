
#include "Projectiles.h"
#include "BlasteroidsAssets.h"
#include "Asteroid.h"
#include "Blasteroids.h"
#include <Utils.h>
#include "Crystal.h"

using namespace igad;

Projectile::Projectile(World& world) : Entity(world)
{
	float s = 1.0f;
	_transform = CreateComponent<Transform>();
	auto& trns = _transform->GetTransform();
	trns = Matrix44::CreateScale(Vector3(s, s, s));

	// Point light
	_light = CreateComponent<Light>();
	_light->SetLightType(Light::POINT_LIGHT);

	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();
	_renderer = CreateComponent<MeshRenderer>();
	_renderer->SetMesh(assets->BoxMesh);
	_renderer->SetShader(assets->AsteroidShader);
	_renderer->SetTexture(assets->WhiteTexture);
	_renderer->SetAmbient(Color::Red);

	_physicsBody = CreateComponent<PhysicsBody2D>();
	_physicsBody->SetMass(10.0f);
	_physicsBody->SetLinearDamping(0.0);
	float sz = 1.5f;
	vector<Vector2> collision =
	{
		Vector2(-sz, -sz),
		Vector2(-sz, sz),
		Vector2(sz, sz),
		Vector2(sz, -sz)
	};
	_physicsBody->SetCollisionShape(move(collision));
}

void Projectile::Init(SpaceShip* spaceship)
{
	auto spcTrasform = spaceship->GetComponent<Transform>();
	auto& transform = GetComponent<Transform>()->GetTransform();

	Vector3 fwd = spcTrasform->GetTransform().GetZAxis();
	Vector3 up(0.0f, 1.0f, 0.0f);
	Vector3 side = up.Cross(fwd);
	transform.SetOrientation(side, up, fwd);
	transform.SetTranslation(spcTrasform->GetPosition() + fwd * 10.0f);

	_physicsBody->SetVelocity( ToVector2(fwd * 220.0f));

	auto blasteroidsWorld = dynamic_cast<Blasteroids*>(&GetWorld());
	Color color = blasteroidsWorld->PlayerColors[spaceship->GetPlayer()];
	_renderer->SetAmbient(color);
	_light->SetColor(color);
}

void Projectile::Update(float dt)
{
	_time += dt;

	if (_time > 1.0f)
		GetWorld().RemoveEntity(this);
}

void Projectile::OnCollision(igad::Collision2D& collision, int body)
{
	Entity& other = body == 0 ? collision.SecondBody->GetEntity() : collision.FirstBody->GetEntity();
	SpaceShip* ship = dynamic_cast<SpaceShip*>(&other);

	if (ship)
	{
		ship->AddHealth(-40.0f);
		GetWorld().RemoveEntity(this);
	}


	Asteroid* asteroid = dynamic_cast<Asteroid*>(&other);

	if (asteroid)
	{
		Blasteroids* blasteroids = dynamic_cast<Blasteroids*>(&GetWorld());
		GetWorld().RemoveEntity(asteroid);
		GetWorld().RemoveEntity(this);
	}

	Crystal* crystal = dynamic_cast<Crystal*>(&other);

	if (crystal)
	{
		Blasteroids* blasteroids = dynamic_cast<Blasteroids*>(&GetWorld());
		GetWorld().RemoveEntity(asteroid);
		return;
	}

	GetWorld().RemoveEntity(this);
}
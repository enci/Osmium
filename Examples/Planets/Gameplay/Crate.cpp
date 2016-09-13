
#include "Crate.h"
#include <Core/Transform.h>
#include <Physics/Physics2D.h>
#include <Graphics/Render.h>
#include <Graphics/MeshRenderer.h>
#include <Utils.h>
#include "BlasteroidsAssets.h"
#include "Projectiles.h"
#include "Blasteroids.h"

using namespace igad;

const float kDensity = 1.0f;


Crate::Crate(World& world) : Entity(world)
{
	_transform = CreateComponent<Transform>();
	_renderer = CreateComponent<MeshRenderer>();
}

Crate::~Crate()
{
}

void Crate::Init(const igad::Vector3& position, const igad::Vector2& velocity)
{
	const float radius = 6.0f;
	_transform->SetPosition(position);
	_transform->SetUniformScale(radius);

	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();
	_renderer->SetMesh(assets->BoxMesh);
	_renderer->SetTexture(assets->WhiteTexture);
	_renderer->SetShader(assets->AsteroidShader);
	_physicsBody = CreateComponent<PhysicsBody2D>();
	_physicsBody->SetRadius(radius);
	_physicsBody->SetMass(DiskArea(radius) * kDensity);
	//_physicsBody->SetLinearDamping(0.0f);
	_wrapper = CreateComponent<Wrapper>();
	float sz = 1.0f; // 4.0f; // *sqrt(2.0f) / 4.0f;
	vector<Vector2> collision =
	{
		Vector2(-sz, -sz),
		Vector2(-sz, sz),
		Vector2(sz, sz),
		Vector2(sz, -sz)
	};
	_physicsBody->SetCollisionShape(move(collision));
}

void Crate::Update(float dt)
{
	if (_wrapper)
		_wrapper->Update(dt);
}

void Crate::OnCollision(igad::Collision2D& collision, int body)
{
	Entity& other = body == 0 ? collision.SecondBody->GetEntity() : collision.FirstBody->GetEntity();

	auto blasteroidsWorld = dynamic_cast<Blasteroids*>(&GetWorld());

	const float maxSize = 3.5f;
	Projectile* projectile = dynamic_cast<Projectile*>(&other);

	if (projectile)
	{
		blasteroidsWorld->GetCamera()->AddShake(3);
		return;
	}

	Crate* crate = dynamic_cast<Crate*>(&other);
	SpaceShip* ship = dynamic_cast<SpaceShip*>(&other);

	if (crate || ship)
	{
		auto particleSystem = blasteroidsWorld->GetParticles();

		int amount = ship ? 2 : 12;

		for (size_t i = 0; i < amount; i++)
		{
			Vector3 rnd(RandInRange(-1.0f, 1.0f),
				RandInRange(-1.0f, 1.0f),
				RandInRange(-1.0f, 1.0f));
			float size = RandInRange(1.4, 4.0f);
			float time = RandInRange(2.0f, 3.5f);
			float speed = RandInRange(2.0f, 5.0f);

			Vector3 axis(RandInRange(-1.0f, 1.0f),
				RandInRange(-1.0f, 1.0f),
				RandInRange(-1.0f, 1.0f));
			axis.Normalize();
			float rotSpeed = RandInRange(-1.0f, 1.0f);

			Vector3 pos = ToVector3(collision.Position) + rnd * 5.0f;
			Color color(255, 255, 255, 50);

			if (ship && RandInRange(0.0f, 1.0f) < 0.5f)
			{
				color = blasteroidsWorld->PlayerColors[ship->GetPlayer()];
			}

			particleSystem->AddParticle(
				pos, rnd * speed,
				axis, rotSpeed,
				color, color,
				size, 0.0f, time);
		}

		float impulse = collision.TotalImpluse();
		float shake = impulse * 0.000001f;
		blasteroidsWorld->GetCamera()->AddShake(shake);

		if (ship)
		{
			float damage = collision.SeparatingVelocity() * 0.3f;
			if (damage < 0)
				ship->AddHealth(damage);
		}
	}
}

#include "Crystal.h"
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


Crystal::Crystal(World& world) : Entity(world)
{
	_transform = CreateComponent<Transform>();
	_renderer = CreateComponent<MeshRenderer>();
}

Crystal::~Crystal()
{
}

void Crystal::Init(const igad::Vector3& position, const igad::Vector2& velocity, float size)
{
	_transform->SetPosition(position);
	_transform->SetUniformScale(size);
	//auto& transform = _transform->GetTransform();
	//`1	qa\ztransform = Matrix44::CreateRotateZ(RandInRange(-Pi, Pi)) * transform;

	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();

	Mesh* mesh = assets->CrystalMesh;

	vector<VertexFormat> vertices = mesh->GetVertices();
	vector<GLushort> indices = mesh->GetIndices();

	
	_collision.clear();
	for (size_t i = 0; i < vertices.size(); i++)
	{
			Vector3& v = vertices[i].Position;
			_collision.push_back(ToVector2(v) * 1.0f);
	}


	_mesh = new Mesh();
	_mesh->SetVertices(move(vertices));
	_mesh->SetIndices(move(indices));
	_mesh->Apply();

	_renderer->SetMesh(_mesh);

	_renderer->SetMesh(_mesh);
	_renderer->SetTexture(assets->WhiteTexture);
	_renderer->SetShader(assets->AsteroidShader);
	_renderer->SetDiffuse(Color::Blue);
	_physicsBody = CreateComponent<PhysicsBody2D>();
	_physicsBody->SetRadius(size);
	_physicsBody->SetMass(DiskArea(size) * kDensity);
	_physicsBody->SetOrientation(RandInRange(-Pi, Pi));
	//_physicsBody->SetLinearDamping(0.0f);
	_wrapper = CreateComponent<Wrapper>();

	vector<Vector2> shape = CreateConvexHull(_collision);
	_physicsBody->SetCollisionShape(move(shape));
	_collision.clear();
}

void Crystal::Update(float dt)
{
	if (_wrapper)
		_wrapper->Update(dt);
}

void Crystal::OnCollision(igad::Collision2D& collision, int body)
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

	Crystal* crate = dynamic_cast<Crystal*>(&other);
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

#include "Ammo.h"
#include "BlasteroidsAssets.h"
#include "Projectiles.h"

using namespace igad;

const float kDensity = 2.0f;

Ammo::Ammo(World& world) : Entity(world)
{
	float radius = 3.0f;
	Vector2 position = Vector2::RandomUnit() * 220.0f;
	_transform = CreateComponent<Transform>();
	_renderer = CreateComponent<MeshRenderer>();
	_transform->SetPosition(ToVector3(position));
	_transform->SetUniformScale(radius);

	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();

	_renderer->SetMesh(assets->AmmoMesh);
	_renderer->SetTexture(assets->WhiteTexture);
	_renderer->SetShader(assets->AsteroidShader);

	auto ammoCoreRenderer = CreateComponent<MeshRenderer>();
	ammoCoreRenderer->SetMesh(assets->AmmoCoreMesh);
	ammoCoreRenderer->SetTexture(assets->WhiteTexture);
	ammoCoreRenderer->SetShader(assets->AsteroidShader);
	ammoCoreRenderer->SetAmbient(Color::Orange * 0.7f);

	auto light = CreateComponent<Light>();
	light->SetLightType(Light::POINT_LIGHT);
	light->SetColor(Color::Orange);

	_physicsBody = CreateComponent<PhysicsBody2D>();
	_physicsBody->SetRadius(radius);
	_physicsBody->SetMass(DiskArea(radius) * kDensity);
	_physicsBody->SetLinearDamping(0.0f);
	_physicsBody->SetVelocity(-0.1f * position);
	_wrapper = CreateComponent<Wrapper>();
	float sz = 1.0f;
	vector<Vector2> collision =
	{
		Vector2(-sz, -sz),
		Vector2(-sz, sz),
		Vector2(sz, sz),
		Vector2(sz, -sz)
	};
	_physicsBody->SetCollisionShape(move(collision));
}

void Ammo::Update(float dt)
{
	if (_wrapper)
		_wrapper->Update(dt);
}

void Ammo::OnCollision(igad::Collision2D& collision, int body)
{
	Entity& other = body == 0 ? collision.SecondBody->GetEntity() : collision.FirstBody->GetEntity();
	SpaceShip* ship = dynamic_cast<SpaceShip*>(&other);

	if (ship)
	{
		ship->AddAmmo(10);
		auto blasteroidsWorld = dynamic_cast<Blasteroids*>(&GetWorld());
		blasteroidsWorld->RemoveEntity(this);
		blasteroidsWorld->CreateAmmo();
	}
}
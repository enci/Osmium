
#include "Push.h"
#include "Spaceship.h"
#include "Blasteroids.h"
#include <Utils.h>

using namespace igad;

Push::Push(World& world) : Entity(world)
{
	_transform = CreateComponent<Transform>();
	_physicsBody = CreateComponent<PhysicsBody2D>();
	_physicsBody->SetMass(FLT_MAX);
	_physicsBody->SetLinearDamping(1.0f);
	_physicsBody->SetRadius(15.0f);
	float sz = 8.0f;
	vector<Vector2> collision =
	{
		Vector2(-sz, -sz),
		Vector2(-sz, sz),
		Vector2(sz, sz),
		Vector2(sz, -sz)
	};
	_physicsBody->SetCollisionShape(move(collision));
}

Push::~Push() {}

void Push::Init(SpaceShip* spaceship)
{
	float radius = _physicsBody->GetRaduis();
	auto spcTrasform = spaceship->GetComponent<Transform>();
	Vector3 fwd = spcTrasform->GetTransform().GetZAxis();
	Vector3 pos = spcTrasform->GetPosition() + fwd * (radius + 5.0f);
	auto& transform = _transform->GetTransform();
	Vector3 up(0.0f, 1.0f, 0.0f);
	Vector3 side = up.Cross(fwd);
	transform.SetOrientation(side, up, fwd);
	transform.SetTranslation(pos);
	_force = fwd * 100000.0f;


	auto world = dynamic_cast<Blasteroids*>(&GetWorld());
	auto particles = world->GetParticles();

	for (size_t i = 0; i < 30; i++)
	{
		Vector3 rnd = RandomOnUnitSphere() * radius;
		rnd.y *= 0.2f;
		float size = RandInRange(0.8, 2.0f);
		float time = RandInRange(0.5f, 1.5f);
		float speed = RandInRange(20.0f, 50.0f);

		Vector3 axis = RandomOnUnitSphere();
		axis.Normalize();
		float rotSpeed = RandInRange(-1.0f, 1.0f);

		Color color = world->PlayerColors[spaceship->GetPlayer()];
		color.a = 180;

		particles->AddParticle(	rnd + pos,
								fwd * speed,
								axis,
								rotSpeed,
								color,
								color,
								size,
								0.0f,
			time);
	}

	world->GetCamera()->AddShake(5.0f);
}

void Push::Update(float dt)
{
	GetWorld().RemoveEntity(this);
}

void Push::OnCollision(igad::Collision2D& collision, int body)
{
	PhysicsBody2D* other = body == 0 ? collision.SecondBody : collision.FirstBody;
	other->AddForce(ToVector2(_force * other->GetRaduis()));
	collision.Resolved = true;
}

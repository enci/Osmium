
#include "Asteroid.h"
#include <Graphics/PerlinNoise.h>
#include <Utils.h>
#include "BlasteroidsAssets.h"
#include "Projectiles.h"
#include "Blasteroids.h"

using namespace igad;

const float kDensity = 1.0f;

Vector2 RandVel()
{
	Vector2 vel;
	do
	{
		vel = Vector2(	RandInRange(-1.0f, 1.0f),
						RandInRange(-1.0f, 1.0f));
	} while (vel.SquareMagnitude() > 1.0);
	vel.Normalize();
	return vel;
}




Asteroid::Asteroid(World& world) : Entity(world)
{
	_transform = CreateComponent<Transform>();
	_renderer = CreateComponent<MeshRenderer>();	
}

Asteroid::~Asteroid()
{
	delete _mesh;
}

void Asteroid::Init(const igad::Vector3& position,
					const igad::Vector2& velocity,
					float radius)
{
	_transform->SetPosition(position);
	_transform->SetUniformScale(radius);

	InitMesh(radius);
	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();
	if(position.y == 0)
		_renderer->SetTexture(assets->WhiteTexture);
	else
		_renderer->SetTexture(assets->GreyTexture);

	_renderer->SetShader(assets->AsteroidShader);

	float rotspeed = velocity.Magnitude() * 0.1f;

	if (position.y == 0)
	{
		_physicsBody = CreateComponent<PhysicsBody2D>();
		_physicsBody->SetPosition(ToVector2(position));
		_physicsBody->SetRadius(radius);
		_physicsBody->SetMass(DiskArea(radius) * kDensity);
		_physicsBody->SetVelocity(velocity);
		_physicsBody->SetLinearDamping(0.0f);
		_physicsBody->SetAngularDamping(0.0f);
		_physicsBody->SetAngularVelocity(RandInRange(-rotspeed, rotspeed));

		InitCollisionShape();
		_wrapper = CreateComponent<Wrapper>();
	}
}

Vector3 Noise(Vector3 p, float offset)
{
	float x = ImprovedNoise::Noise(p.x, p.y, p.z) * offset;
	float y = ImprovedNoise::Noise(p.y, p.z, p.x) * offset;
	float z = ImprovedNoise::Noise(p.z, p.x, p.y) * offset;
	return Vector3(x, y, z);
}

void Asteroid::InitMesh(float radius)
{
	auto assets = GetWorld().GetComponent<BlasteroidsAssets>();

	int idx =  (radius - 3) * 0.6f;
	if (idx < 0) idx = 0;
	if (idx > 5) idx = 5;
	Mesh* mesh = assets->GeoSpheres[idx];

	vector<VertexFormat> vertices = mesh->GetVertices();
	vector<GLushort> indices = mesh->GetIndices();

	Vector3 randomSampleOffset(	RandInRange(-Pi, Pi),
								RandInRange(-Pi, Pi),
								RandInRange(-Pi, Pi));
	float offset = 0.6f;

	_collision.clear();
	for (size_t i = 0; i < vertices.size(); i++)
	{
		Vector3& v = vertices[i].Position;
		v += Noise(v + randomSampleOffset, offset);
		_collision.push_back(ToVector2(v) * 1.0f);
	}

	for (size_t baseIdx = 0; baseIdx < indices.size() / 3; baseIdx++)
	{
		Vector3& v0 = vertices[baseIdx * 3 + 0].Position;
		Vector3& v1 = vertices[baseIdx * 3 + 1].Position;
		Vector3& v2 = vertices[baseIdx * 3 + 2].Position;

		Vector3 v10 = v1 - v0;
		Vector3 v20 = v2 - v0;
		Vector3 normal = v10.Cross(v20);
		normal.Normalize();

		for (auto i = 0; i < 3; i++)
			vertices[baseIdx * 3 + i].Normal = normal;
	}

	_mesh = new Mesh();
	_mesh->SetVertices(move(vertices));
	_mesh->SetIndices(move(indices));
	_mesh->Apply();

	_renderer->SetMesh(_mesh);
}

void Asteroid::InitCollisionShape()
{	
	vector<Vector2> shape = CreateConvexHull(_collision);
	_physicsBody->SetCollisionShape(move(shape));
	_collision.clear();
}

void Asteroid::Explode(Entity& other, int amount)
{
	auto blasteroidsWorld = dynamic_cast<Blasteroids*>(&GetWorld());
	auto particleSystem = blasteroidsWorld->GetParticles();
	//Vector3 center = _transform->GetPosition();
	Vector3 center = other.GetComponent<Transform>()->GetPosition();

	for (size_t i = 0; i < amount; i++)
	{
		Vector3 rnd(RandInRange(-1.0f, 1.0f), RandInRange(-1.0f, 1.0f), RandInRange(-1.0f, 1.0f));
		rnd.y *= 0.2f;
		float size = RandInRange(1.8, 5.0f);
		float time = RandInRange(2.0f, 3.5f);
		float speed = RandInRange(10.0f, 30.0f);

		Vector3 axis(RandInRange(-1.0f, 1.0f), RandInRange(-1.0f, 1.0f), RandInRange(-1.0f, 1.0f));
		axis.Normalize();
		float rotSpeed = RandInRange(-1.0f, 1.0f);

		Vector3 pos = center + rnd * 5.0f;

		Color toColor = Color::White;
		toColor.a *= 0.2f;

		Color fromColor0("ff8126");
		Color fromColor1("ff0000");
		Color fromColor = Lerp(fromColor0, fromColor1, RandInRange(0.0f, 0.5f));
		particleSystem->AddParticle(pos, rnd * speed,
									axis, rotSpeed, fromColor, toColor, size, 0.0f, time);
	}
}

void Asteroid::Update(float dt)
{
	if(_wrapper)
		_wrapper->Update(dt);
}

void Asteroid::OnCollision(igad::Collision2D& collision, int body)
{
	Entity& other = body == 0 ? collision.SecondBody->GetEntity() : collision.FirstBody->GetEntity();

	auto blasteroidsWorld = dynamic_cast<Blasteroids*>(&GetWorld());

	const float maxSize = 3.5f;
	Projectile* projectile = dynamic_cast<Projectile*>(&other);

	if (projectile)
	{
		// Split the sphere in two
		float ratio = RandInRange(0.25f, 0.75f);

		// Get the volume
		float A = DiskArea(_physicsBody->GetRaduis()) * 0.5f;
		float r0 = DiskRadius(A * ratio);
		float r1 = DiskRadius(A - (1.0f - ratio));

		Vector2 pos = ToVector2(_transform->GetPosition());

		Vector2 projVel = other.GetComponent<PhysicsBody2D>()->GetVelocity();
		float speed = projVel.Magnitude() * 0.1f;

		if (r0 > maxSize)
		{
			Vector2 vel = RandVel();
			Asteroid* a0 = GetWorld().CreateEntity<Asteroid>();
			a0->Init(ToVector3(pos + vel * 0.1f), vel * speed, r0);
		}
		if (r1 > maxSize)
		{
			Vector2 vel = RandVel();
			Asteroid* a1 = GetWorld().CreateEntity<Asteroid>();
			a1->Init(ToVector3(pos + vel * 0.1f), vel  * speed, r1);
		}

		GetWorld().RemoveEntity(this);
		GetWorld().RemoveEntity(projectile);

		Explode(other, _physicsBody->GetRaduis() * 5.0f);
		blasteroidsWorld->GetCamera()->AddShake(3);
		return;
	}

	Asteroid* asteroid = dynamic_cast<Asteroid*>(&other);
	SpaceShip* ship = dynamic_cast<SpaceShip*>(&other);

	if (asteroid || ship)
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
			if(damage < 0)
				ship->AddHealth(damage);
		}
	}
}
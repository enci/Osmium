
#include "Wrapper.h"
#include <Core/World.h>
#include "Blasteroids.h"
#include <Graphics/DebugRenderer.h>

using namespace igad;

Wrapper::Wrapper(igad::Entity& entity) : Component(entity)
{
	_transform = GetEntity().GetComponent<Transform>();
	_body = GetEntity().GetComponent<PhysicsBody2D>();
}

/*
void Wrapper::Update(float dt)
{
	auto world = dynamic_cast<Blasteroids*>(&GetEntity().GetWorld());
	float width = world->WorldWidth;
	float height = world->WorldHeight;

	if(_body)
	{
		width += _body->GetRaduis();
		height += _body->GetRaduis();
	}

	Vector3 position = _transform->GetPosition();

	if (position.x > width)
		position.x -= 2 * width;
	else if (position.x < -width)
		position.x += 2 * width;

	if (position.z > height)
		position.z -= 2 * height;
	else if (position.z < -height)
		position.z += 2 * height;

	_transform->SetPosition(position);
}
*/

void Wrapper::Update(float dt)
{
	_cooldown -= dt;
	if (_cooldown > 0.0f)
		return;

	if(!_camera)
		_camera = GetEntity().GetWorld().GetEntitiesByType<BlasteroidsCamera>()[0];

	float f = 1.0f;
	vector<Vector2> corners = 
	{
		CalculateGameWorldProjection(Vector2(-1, -1) * f),
		CalculateGameWorldProjection(Vector2(-1, 1) * f),
		CalculateGameWorldProjection(Vector2(1, 1) * f),
		CalculateGameWorldProjection(Vector2(1, -1) * f)
	};


	for (size_t i = 0; i < 4; i++)
	{
		gDebugRenderer.AddLine(ToVector3(corners[(i + 1) % 4]), ToVector3(corners[i]));

		Vector2 edge = corners[(i + 1) % 4] - corners[i];
		Vector2 axis = edge.Perpendicular();
		axis.Normalize();

		float edgeProjection = axis.Dot(corners[i]);
		float bodyProjection = axis.Dot(_body->GetPosition());

		if (bodyProjection - edgeProjection > _body->GetRaduis())
		{
			_cooldown = 2;

			gDebugRenderer.AddCircle(ToVector3(_body->GetPosition()), _body->GetRaduis(), Color::Magenta);
			Vector2 toPoint = _body->GetPosition() - corners[i];
			float t = toPoint.Dot(edge) / edge.Dot(edge);
			
			Vector2 proj = Lerp(corners[(i + 2) % 4], corners[(i + 3) % 4], 1.0f - t);
			proj += axis * -_body->GetRaduis();

			_body->SetPosition(proj);
			gDebugRenderer.AddCircle(ToVector3(proj), 2, Color::White);
		}
	}
}

igad::Vector2 Wrapper::CalculateGameWorldProjection(const igad::Vector2 canonical) const
{
	Ray3D ray = _camera->Unproject(canonical);
	float t = -ray.Origin.y / ray.Direction.y;
	Vector3 point = ray.Evaluate(t);
	return ToVector2(point);
}

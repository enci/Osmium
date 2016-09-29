#include <Physics/Physics2D.h>
#include <Core/Entity.h>
#include <Core/World.h>
#include <Core/Transform.h>
#include <algorithm>
#include <vector>
#include <Graphics/DebugRenderer.h>
#include <Utils.h>
#include <Defines.h>

using namespace Osm;
const float skin = 0.1f;

void AABB::Clear()
{
	Min.x = FLT_MAX;
	Min.y = FLT_MAX;
	Max.x = -FLT_MAX;
	Max.y = -FLT_MAX;
}

void AABB::AddPoint(const Vector2& point)
{
	// X
	if (point.x > Max.x)
		Max.x = point.x;
	if (point.x < Min.x)
		Min.x = point.x;

	// Y
	if (point.y > Max.y)
		Max.y = point.y;
	if (point.y < Min.y)
		Min.y = point.y;
}

void AABB::DebugRender() const
{
	Color color = Color::Orange;
	Vector3 A(Min.x, 0.0f, Min.y);
	Vector3 B(Max.x, 0.0f, Min.y);
	Vector3 D(Min.x, 0.0f, Max.y);
	Vector3 C(Max.x, 0.0f, Max.y);
	gDebugRenderer.AddLine(A, B, color);
	gDebugRenderer.AddLine(B, C, color);
	gDebugRenderer.AddLine(C, D, color);
	gDebugRenderer.AddLine(D, A, color);
}

bool AABB::IsValid() const
{
	return	Min.x != FLT_MAX && Min.y != FLT_MAX &&
			Max.x != -FLT_MAX &&  Max.y != -FLT_MAX;
}

PhysicsBody2D::PhysicsBody2D(Entity& entity)
	: Component(entity)
{
	auto pm = entity.GetWorld().GetComponent<PhysicsManager2D>();
	ASSERT(pm);
	pm->AddPhysicsBody(this);

	_transform = GetOwner().GetComponent<Transform>();
	ASSERT(_transform);
	_position = ToVector2(_transform->GetPosition());

	_orientation = 0.0f;
	_angularVelocity = 0.0f;
	_size = Vector2(-1.0f, -1.0f);
	_torque = 0.0f;
	_angularDamping = 1.0f;
}

PhysicsBody2D::~PhysicsBody2D()
{
	auto pm = GetOwner().GetWorld().GetComponent<PhysicsManager2D>();
	pm->RemovePhysicsBody(this);
}

Vector2 PhysicsBody2D::GetVelocityAtPoint(const Vector2& point) const
{
	Vector2 ap = point - _position;
	return _velocity + _angularVelocity * ap.Perpendicular();
}

void Osm::PhysicsBody2D::AddForceAtWorldPoint(const Vector2& f, const Vector2& p)
{
	Vector2 toP = p - _position;
	_torque += toP.Cross(f);
	_force += f;

	gDebugRenderer.AddLine(ToVector3(p), ToVector3(p+f), Color::Purple);
}

void Osm::PhysicsBody2D::AddForceAtLocalPoint(const Vector2& f, const Vector2& p)
{
	Vector2 pt = _matrix.TransformVector(p);
	AddForceAtWorldPoint(f, pt);
}

void PhysicsBody2D::UpdateBody(float dt)
{
	_position = ToVector2(_transform->GetPosition());
	_size = Vector2(
		_transform->GetTransform().GetXAxis().Magnitude(),
		_transform->GetTransform().GetZAxis().Magnitude());
	if (!_initialized)
	{
		_initialized = true;
		UpdateDerived();
	}

	Vector2 accel = _force / _mass;
	_velocity += accel * dt;
	_velocity -= _velocity * (dt * _linearDamping);
	_position += _velocity * dt;
	_force.Clear();

	float rotAccl = _torque / _momentOfInertia;
	_angularVelocity += rotAccl * dt;
	_angularVelocity -= _angularVelocity * (dt * _angularDamping);
	_orientation += _angularVelocity * dt;
	_torque = 0.0f;

	UpdateDerived();
}

void PhysicsBody2D::SetCollisionShape(std::vector<Vector2>&& shape)
{
	_collisionShape = move(shape);
}

const vector<Vector2>& PhysicsBody2D::GetCollisionShape() const
{
	return _collisionShape;
}

const vector<Vector2>& PhysicsBody2D::GetCollisionShapeWorld() const
{
	return _collisionShapeWorld;
}

void Osm::PhysicsBody2D::SetPosition(const Vector2& position)
{
	_position = position;
	_transform->SetPosition(ToVector3(position));
}

void PhysicsBody2D::UpdateDerived()
{	
	if (!_initialized)
		return;

	Vector2 direction = Vector2(sin(_orientation), cos(_orientation));
	_matrix.SetTransform(
		direction,
		_size,
		_position);
	_inverse = _matrix.Inverse();


	if (_collisionShape.size() == 0)
		return;

	auto n = _collisionShape.size();
	_collisionShapeWorld.resize(n);

	float maxSqDist = -FLT_MAX;
	Vector2 centerWorld = _matrix.TransformVector(Vector2());

	_boundingBox.Clear();

	for (size_t i = 0; i < n; i++)
	{
		Vector2& v = _collisionShape[i];
		_collisionShapeWorld[i] = _matrix.TransformVector(v);
		float distSq = (_collisionShapeWorld[i] - centerWorld).SquareMagnitude();
		if (distSq > maxSqDist)
			maxSqDist = distSq;

		_boundingBox.AddPoint(_collisionShapeWorld[i]);
	}

	_radius = sqrt(maxSqDist);

	_momentOfInertia = _mass * Sqr(_radius) / 2;
}

void PhysicsBody2D::UpdateTransform()
{
	if (!_initialized)
		return;

	UpdateMatrix44FromMatrix33(_transform->GetTransform(), _matrix);
	DebugRenderShape();
}

void PhysicsBody2D::DebugRenderShape()
{
	auto& shape = _collisionShapeWorld;

	auto n = shape.size();
	Color color(105, 105, 105);
	
	for (auto i = 0; i < n; i++)
	{
		Vector2& v0 = shape[i];
		Vector2& v1 = shape[(i + 1) % n];

		gDebugRenderer.AddLine( ToVector3(v0), ToVector3(v1), color);
	}

	//gDebugRenderer.AddCircle(_transform->GetPosition(), _radius * 1.02f);
	//_boundingBox.DebugRender();
}

PhysicsManager2D::PhysicsManager2D(World& world)
	: Component(world)
{
}

void PhysicsManager2D::UpdatePhysics(float dt)
{
	for (auto b : _bodies)
		b->UpdateBody(dt);

	AccumulateContacts();
	CallOnCollisionEvent();
	ResloveCollisions();

	for (auto b : _bodies)
	{
		b->UpdateDerived();
		b->UpdateTransform();
	}
}

void PhysicsManager2D::AddPhysicsBody(PhysicsBody2D* body)
{
	_bodies.push_back(body);
}

void PhysicsManager2D::RemovePhysicsBody(PhysicsBody2D* body)
{
	_bodies.erase(remove(_bodies.begin(), _bodies.end(), body));
}

//
// Collision detection
//
float FindMin(const Vector2& axis, const vector<Vector2>& shape)
{
	float minimum = FLT_MAX;
	for (size_t i = 0; i < shape.size(); i++)
	{
		float projection = axis.Dot(shape[i]);
		if (projection < minimum)
			minimum = projection;
	}

	return minimum;
}

Vector2 FindSupport(const Vector2& axis, const vector<Vector2>& shape)
{
	float minimum = FLT_MAX;
	vector<Vector2> support(2);

	for (size_t i = 0; i < shape.size(); i++)
	{
		float projection = axis.Dot(shape[i]);

		if (abs(projection - minimum) < 0.1f) 
		{
			support.push_back(shape[i]);
		}
		else if(projection < minimum)
		{
			support.clear();
			support.push_back(shape[i]);
			minimum = projection;
		}
	}

	Vector2 supportPoint = support.size() >= 2 ? (support[0] + support[1]) * 0.5f : support[0];

	gDebugRenderer.AddCircle(ToVector3(supportPoint), 0.5, Color::Orange, 3);

	return supportPoint;
}

float CheckProjection(	const vector<Vector2>& fromShape,
						const vector<Vector2>& toShape,
						float min,
						Vector2& normal,
						size_t& axisFrom,
						size_t& axisTo)
{
	size_t n = fromShape.size();
	for (size_t i = 0; i < n; i++)
	{
		Vector2 axis = fromShape[i] - fromShape[(i + 1) % n];
		if (axis.SquareMagnitude() > 0.0)
		{
			axis.Normalize();
			axis = axis.Perpendicular();
			float proj0 = axis.Dot(fromShape[i]);
			float proj1 = FindMin(axis, toShape);
			float overlap = proj0 - proj1;

			if (overlap <= 0.0f)
				return overlap;

			if (overlap < min)
			{
				min = overlap;
				normal = axis;
				axisFrom = i;
				axisTo = (i + 1) % n;
			}
		}
	}

	return min;
}

bool CheckCollision(PhysicsBody2D* body0, PhysicsBody2D* body1, Collision2D& collision)
{
	auto& shape0 = body0->GetCollisionShapeWorld();
	auto& shape1 = body1->GetCollisionShapeWorld();

	size_t axisFrom;
	size_t axisTo;
	Vector2 normal;
	float min0 = CheckProjection(shape0, shape1, FLT_MAX, normal, axisFrom, axisTo);
	if(min0 < 0.0f)
		return false;
	float min1 = CheckProjection(shape1, shape0, min0, normal, axisFrom, axisTo);
	if (min1 < 0.0f)
		return false;

	if(min1 < min0)
	{
		swap(body0, body1);
	}

	auto& shape = body0->GetCollisionShapeWorld();

	Vector2 from = shape[axisFrom];
	Vector2 to = shape[axisTo];
	
	float overlap = min1 < min0 ? min1 : min0;

	// Uncoment to debug axis and it's normal/overlap
	//gDebugRenderer.AddLine(ToVector3(from), ToVector3(to), Color::Yellow);
	//gDebugRenderer.AddLine(ToVector3((from+to) * 0.5f), ToVector3((from + to) * 0.5f - normal * overlap), Color::Yellow);

	collision.FirstBody = body0;
	collision.SecondBody = body1;
	collision.Normal = -1 * normal;
	collision.Overlap = overlap;
	collision.Position1 = FindSupport(normal, body1->GetCollisionShapeWorld());
	collision.Position0 = collision.Position1 + normal * overlap;
	collision.Position = (collision.Position0 + collision.Position0) * 0.5f;

	Vector2 vel0 = collision.FirstBody->GetVelocityAtPoint(collision.Position0);
	gDebugRenderer.AddLine(ToVector3(collision.Position0), ToVector3(collision.Position0 + vel0), Color::Purple);

	Vector2 vel1 = collision.SecondBody->GetVelocityAtPoint(collision.Position1);
	gDebugRenderer.AddLine(ToVector3(collision.Position1), ToVector3(collision.Position1 + vel1), Color::Purple);

	//gDebugRenderer.AddLine(ToVector3(collision.Position0), ToVector3(collision.Position1), Color::Orange);

	return true;
}
 
bool Overlap(const AABB left, const AABB& right)
{
	return	(left.Min.x <= right.Max.x) && (left.Min.y <= right.Max.y) &&
			(left.Max.x >= right.Min.x) && (left.Max.y >= right.Min.y);
}

void PhysicsManager2D::AccumulateContacts()
{
	_collisions.clear();

	for (size_t i = 0; i < _bodies.size(); i++)
	{
		auto body0 = _bodies[i];

		for (size_t j = i + 1; j < _bodies.size(); j++)
		{
			auto body1 = _bodies[j];

			const AABB& box0 = body0->GetBoundingBox();
			const AABB& box1 = body1->GetBoundingBox();

			if (!box0.IsValid() || !box0.IsValid())
				break;

			if (Overlap(box0, box1))
			{
				Collision2D collision; // Blank (invalid) collision 

				if (CheckCollision(body0, body1, collision))
				{
					_collisions.push_back(collision);
				}
			}
		}
	}
}



void PhysicsManager2D::CallOnCollisionEvent()
{
	for (size_t i = 0; i < _collisions.size(); i++)
	{
		Collision2D& collision = _collisions[i];
		Entity& e0 = collision.FirstBody->GetOwner();
		Entity& e1 = collision.SecondBody->GetOwner();
		e0.OnCollision(collision, 0);
		e1.OnCollision(collision, 1);
	}
}

void PhysicsManager2D::ResloveCollisions()
{
	for (size_t i = 0; i < _collisions.size(); i++)
	{
		Collision2D& collision = _collisions[i];

		if (!collision.Resolved)
		{
			PhysicsBody2D& first = *collision.FirstBody;
			PhysicsBody2D& second = *collision.SecondBody;


			//
			// Resolve overlap
			//
			float totalMass = first._mass + second._mass;

			// Calculate amount of penetration resoluion per total mass
			Vector2 movePerMass = collision.Normal * (collision.Overlap / totalMass);

			first._position += movePerMass * second.GetMass();
			second._position -= movePerMass * first.GetMass();

			first._position += collision.Normal * skin;
			second._position -= collision.Normal * skin;

			//
			// Resolve impuleses
			//

			// Get velocity of point contact on first body
			Vector2 rAP = collision.Position0 - first._position;
			rAP = rAP.Perpendicular();
			Vector2 velA = first._velocity + first._angularVelocity * rAP;

			// Get velocity of point contact on first body
			Vector2 rBP = collision.Position1 - second._position;
			rBP = rBP.Perpendicular();
			Vector2 velB = second._velocity + second._angularVelocity * rBP;
			
			Vector2 relativeVelocity = velA - velB;

			// Get the velocity in the direction of the contact
			float separatingVelocity = relativeVelocity.Dot(collision.Normal);

			// Check if there is anything to resolve
			if (separatingVelocity > 0)
			{
				// Contact separating or stationary, move along nothing to see
				continue;
			}

			float restitution = (collision.FirstBody->GetRestitutuion() + collision.SecondBody->GetRestitutuion()) * 0.5f;
			float inverseMass = 1 / first._mass + 1 / second._mass;
			float denominator = Sqr(rAP.Dot(collision.Normal)) / first._momentOfInertia +
								Sqr(rBP.Dot(collision.Normal)) / second._momentOfInertia +
								inverseMass;
			float impulse = (-(1 + restitution) * separatingVelocity) / denominator;
			

			first._velocity += (impulse / first._mass) * collision.Normal;
			second._velocity -= (impulse / second._mass) * collision.Normal;

			first._angularVelocity += rAP.Dot(impulse * collision.Normal) / first._momentOfInertia;
			second._angularVelocity -= rBP.Dot(impulse * collision.Normal) / second._momentOfInertia;
		}
	}
}


//
//	Convex hull
//


// Three points are a counter - clockwise turn if ccw > 0, clockwise if
// ccw < 0, and collinear if ccw = 0 because ccw is a determinant that
// gives twice the signed  area of the triangle formed by p1, p2 and p3.
float CCW(Vector2 p1, Vector2 p2, Vector2 p3)
{
	return (p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
}

// A very litteral implentation of the graham scanl algo 
std::vector<Vector2> GrahamScanCovexHull(vector<Vector2> vertices)
{
	if (vertices.size() < 3)
		return vertices;

	int min = 0;
	for (size_t i = 1; i < vertices.size(); i++)
	{
		if (vertices[i].y < vertices[min].y)
			min = i;
	}
	swap(vertices[min], vertices[0]);

	Vector2& pivot = vertices[0];
	Vector2 base(1, 0);
	
	sort(vertices.begin() + 1, vertices.end(),
		[pivot, base](const Vector2& v0, const Vector2& v1)
		{
			Vector2 t0 = v0 - pivot;
			Vector2 t1 = v1 - pivot;

			float d0 = atan2(t0.y, t0.x);
			float d1 = atan2(t1.y, t1.x);

			return d0 > d1;
		}
	);

	vertices.erase(unique(vertices.begin(), vertices.end()), vertices.end());

	vector<Vector2> hull;
	hull.push_back(vertices[0]);
	hull.push_back(vertices[1]);	
	vertices.push_back(vertices[0]);
	//
	for (size_t i = 2; i < vertices.size(); i++)
	{
		Vector2& n = vertices[i];
		do
		{
			size_t m = hull.size();
			Vector2& p = hull[m - 2];
			Vector2& c = hull[m - 1];
			if (CCW(p, c, n) > 0)
				hull.resize(m - 1);
			else
				break;
		}
		while (true);
		hull.push_back(n);
	}

	return hull;
}

std::vector<Vector2> Osm::CreateConvexHull(const std::vector<Vector2>& vertices)
{
	return GrahamScanCovexHull(vertices);
}
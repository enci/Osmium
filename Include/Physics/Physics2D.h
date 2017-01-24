#pragma once

#include <Core/Entity.h>
#include <Physics/Collision2D.h>
#include <Core/Transform.h>
#include <Utils.h> 
#include <Math/Matrix33.h>
#include <unordered_map>

namespace Osm
{

///
/// Axis aligned bouding box
///
struct AABB
{
	/// Min (bottom left)
	Vector2 Min;

	/// Max (top right)
	Vector2 Max;

	/// Clear the AABB by setting it to invalid size 
	void Clear();

	/// Add a point to the AABB. If inside, extends the bound, otherwise nothing.
	void AddPoint(const Vector2& point);

	/// Debug renders the AABB in orange.
	void DebugRender() const;

	/// Check if valid
	bool IsValid() const;
};

/// 
/// Rotation
///
struct Rotation
{
	/// cos(a)
	float c;

	/// -sin(a)
	float s;	

	/// Ctor at angle 0
	Rotation() { c = 1, s = 0; }

	// Mat33 GetRotationMatrix();

	float GetAngle();

	float SetAngle(float angle);

	float AddAngle(float angle);
};

///
/// A physics body, has collision/detection functionality as well as forces/movement
/// simulation. Gets initialized from the transoform (3D transform) and drives the
/// simulation from there on.
///
class PhysicsBody2D : public Component<Entity>
{
	// Needed for certain functionality
	friend class PhysicsManager2D;

public:
	/// Create a new physics body
	PhysicsBody2D(Entity& entity);

	/// Delete body
	virtual ~PhysicsBody2D();

	/// Get the position of the body (will be applied to the transform next frame) 
	const Vector2& GetPosition() const { return _position; }

	/// Set the position of the body (will be applied to the transform next frame) 
	void SetPosition(const Vector2& position);

	/// Get orientation in radians
	float GetOrientation() const { return _orientation; }

	/// Set orientation in radians
	void SetOrientation(float orientation) { _orientation = orientation; }

	/// Get current velocity
	const Vector2&	GetVelocity() const { return _velocity; }

	/// Set the velocity
	void SetVelocity(const Vector2& vel) { _velocity = vel; }

	/// Get the speed (magnitude of velocity)
	float GetSpeed() const { return _velocity.Magnitude(); }

	/// Get the angular velocity (rad/s)
	float GetAngularVelocity() const { return _angularVelocity; }

	/// Set the angular velocity (rad/s)
	void SetAngularVelocity(float av) {  _angularVelocity = av; }

	/// Gets the velocity on a point of the body
	/// Any point in world coordinates can be given.
	Vector2 GetVelocityAtPoint(const Vector2& point) const;

	/// Gets linear damping
	/// 0 -> Undamped (friction) motion
	/// 1 -> No motion, all forces dampened
	float   GetLinearDamping() const { return _linearDamping; }

	/// Set linear damping. Fake friction
	/// 0 -> Undamped (friction) motion
	/// 1 -> No motion, all forces dampened
	void    SetLinearDamping(float damping) { _linearDamping = damping; }

	/// Gets angular damping
	/// 0 -> Undamped (friction) motion
	/// 1 -> No motion, all torques dampened
	float   GetAngularDamping() const { return _angularDamping; }

	/// Set angular damping. Fake friction
	/// 0 -> Undamped (friction) motion
	/// 1 -> No motion, all torques dampened
	void    SetAngularDamping(float damping) { _angularDamping = damping; }

	/// Get radius, recalulated from the polygon collision shape
	float   GetRadius() const { return _radius; }

	/// Will get recalulated from the polygon collision shape
	void    SetRadius(float r) { _radius = r; }

	/// Add force through the center of mass (no rotation motion)
	void    AddForce(const Vector2& f) { _force += f; }

	/// Add force at a point
	void    AddForceAtWorldPoint(const Vector2& f, const Vector2& p);

	/// Add force at a point
	void    AddForceAtLocalPoint(const Vector2& f, const Vector2& p);

	/// Add torque
	void	AddTorque(float t) { _torque += t; }

	/// Get mass
	float   GetMass() const { return  _mass; }

	/// Set mass
	void    SetMass(float m) { _mass = m; }

	/// Get restitution factor
	float   GetRestitutuion() const { return  _restitution; }

	/// Set restitution factor
	void    SetRestitutuion(float r) { _restitution = r; }

	/// Calculate world space vector
	Vector2 GetToWorld(const Vector2& local) const;

	/// Get the forward vector
	Vector2 GetForward() const;

	/// Update body - integrate forces
	void UpdateBody(float dt);

	/// Set collision shape
	/// @param shap Covex polygon.
	void SetCollisionShape(std::vector<Vector2>&& shape);

	/// Get collision shape in local coordinates
	const std::vector<Vector2>& GetCollisionShape() const;

	/// Get collision shape in world coordinates
	const std::vector<Vector2>& GetCollisionShapeWorld() const;

	/// Get bounding box
	const AABB& GetBoundingBox() const { return _boundingBox; }

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

private:

	/// Updates the bounding volumes
	void UpdateDerived();

	/// Updates the transform from position, scale and orientation
	void UpdateTransform();

	/// Render the shape and the bounding volumes
	void DebugRenderShape();

	float		_radius = 1.0f;
	float		_mass = 1.0f;
	float		_momentOfInertia = 1.0f;
	float		_linearDamping = 0.02f;
	float		_angularDamping = 1.2f;
	float		_restitution = 0.8f;
	Vector2		_size;

	Vector2		_position;
	Vector2		_velocity;
	Vector2		_force;

	float		_orientation;
	float		_angularVelocity;
	float		_torque;
	
	Transform*	_transform = nullptr;
	AABB		_boundingBox;
	Matrix33	_matrix;
	Matrix33	_inverse;
	bool		_initialized = false;

	// Must be a convex shape
	std::vector<Vector2> _collisionShape;
	std::vector<Vector2> _collisionShapeWorld;
};

///
///
///
class MultiGrid
{
public:
	MultiGrid(const std::vector<PhysicsBody2D*>& bodies);

	std::vector<PhysicsBody2D*> GetNeighbours(PhysicsBody2D* body);

	//std::vector<PhysicsBody2D*> GetNeighbours(PhysicsBody2D* body, float radius);

#ifdef DEBUG
	void DebugRender();
#endif

private:

	int64_t GetIndex(const Vector2& pos);

	float _cellSize = 12.0f;

	std::unordered_map<int64_t, std::vector<PhysicsBody2D*>> _grid;
};


class AutoGrid
{
public:
	AutoGrid(const std::vector<PhysicsBody2D*>& bodies);

	std::vector<PhysicsBody2D*> GetNeighbours(PhysicsBody2D* body);

	//std::vector<PhysicsBody2D*> GetNeighbours(PhysicsBody2D* body, float radius);

#ifdef DEBUG
	void DebugRender();
#endif

private:

	int _gridx;
	int _gridy;

	Vector2 _min;
	Vector2 _max;
	std::vector<std::vector<std::vector<PhysicsBody2D*>>> _grid;
};


class PhysicsManager2D : public Component<World>
{
public:
	PhysicsManager2D(World& world);

	/// Integrate, get collisions, run events and resolve collisions
	void UpdatePhysics(float dt);

	/// Add a body to the manager. No need to ever call this, it's automatic
	void AddPhysicsBody(PhysicsBody2D* body);

	/// Removes a body from the manager. No need to ever call this, it's automatic
	void RemovePhysicsBody(PhysicsBody2D* body);

	/// Get all bodies in the specified reariuis arround the given postion
	std::vector<PhysicsBody2D*> GetInRadius(const Vector2& position, float radius);

	/// A choice of algorithms for accumulating contacts
	enum ContactsAlgorithm { CA_BRUTE_FORCE = 0, CA_AUTO_GRID = 1, CA_MULTI_GRID = 2 };

	/// Set the contacts algorithm
	void SetContactsAlgorithm(ContactsAlgorithm a) { _algorithm = a; }

#ifdef INSPECTOR	
	virtual void Inspect() override;
#endif

private:
	/// Get all the collisions this frame
	void AccumulateContacts();

	/// Get collisions using brute force
	void AccumulateContactsBruteForce();

	/// Get collisions using brute force
	void AccumulateContactsAutoGrid();

	/// Get all bodies in the specified reariuis arround the given postion
	std::vector<PhysicsBody2D*> GetInRadiusBrute(const Vector2& position, float radius);

	/// Get all bodies in the specified reariuis arround the given postion
	std::vector<PhysicsBody2D*> GetInRadiusAutoGrid(const Vector2& position, float radius);

	/// Get all bodies in the specified reariuis arround the given postion
	std::vector<PhysicsBody2D*> GetInRadiusMultiGrid(const Vector2& position, float radius);

	/// Call events on all entities
	void CallOnCollisionEvent();

	/// Resolve overlap and velocity
	void ResloveCollisions();

	/// All the bodies to be simulated
	std::vector<PhysicsBody2D*>	_bodies;

	/// Current collision
	std::vector<Collision2D>	_collisions;

	ContactsAlgorithm			_algorithm = CA_BRUTE_FORCE;

	//Grid						_grid;
};

std::vector<Vector2> CreateConvexHull(const std::vector<Vector2>& vertices);

}

// Defined in header so that it can be inlined
inline int64_t Osm::MultiGrid::GetIndex(const Vector2 & pos)
{
	int64_t i = (int64_t)round(pos.x / _cellSize);
	int64_t j = (int64_t)round(pos.y / _cellSize);
	int64_t idx = i & 0x00000000FFFFFFFF | (j << 32);
	return idx;
}
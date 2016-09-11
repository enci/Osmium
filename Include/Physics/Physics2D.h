#pragma once
#include <Entity.h>
#include <Collision2D.h>
#include <Transform.h>
#include <Utils.h> 
#include <Matrix33.h>

namespace igad
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
	float   GetRaduis() const { return _radius; }

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


class PhysicsManager2D : public Component<World>
{
public:
	PhysicsManager2D(World& world);

	/// Integrate, get collisions, run events and resolve collisions
	void UpdatePhysics(float dt);
		 
	void AddPhysicsBody(PhysicsBody2D* body);
	void RemovePhysicsBody(PhysicsBody2D* body);

private:
	/// Get all the collisions this frame
	void AccumulateContacts();

	/// Call events on all entities
	void CallOnCollisionEvent();

	/// Resolve overlap and velocity
	void ResloveCollisions();

	/// All the bodies to be simulated
	std::vector<PhysicsBody2D*>	_bodies;

	/// Current collision
	std::vector<Collision2D>	_collisions;
};

std::vector<Vector2> CreateConvexHull(const std::vector<Vector2>& vertices);

}

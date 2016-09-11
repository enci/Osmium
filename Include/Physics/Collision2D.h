#pragma once

#include <Vector2.h>

namespace igad
{

class PhysicsBody2D;

///
/// A struct that encapsulates the contact between two  bodies.
///
struct Collision2D
{
	/// Holds the bodies that are involved in the contact. The
	/// second of these can be null, for contacts with the scenery.
	PhysicsBody2D* FirstBody;

	PhysicsBody2D* SecondBody;

	/// Direction of the contact in world coordinates.
	Vector2 Normal;

	/// Depth of penetration at the contact.
	float Overlap;

	/// Position of the contact itself
	Vector2 Position;

	/// Position of the contact itself
	Vector2 Position0;

	/// Position of the contact itself
	Vector2 Position1;

	/// A mechanism to detect collisions to raise event but
	/// not reslove in the physics engine
	bool Resolved;

	/// Default init ctor
	Collision2D()
		: FirstBody(nullptr)
		, SecondBody(nullptr)
		, Normal(Vector2())
		, Overlap(0.0f)
		, Position(Vector2())
		, Resolved(false)
	{}

	/// Creates a new contact among two dynamic bodies
	Collision2D(PhysicsBody2D* firstBody,
				PhysicsBody2D* secondBody,
				Vector2 contactNormal,
				float overlap,
				Vector2 position)
		: FirstBody(firstBody)
		, SecondBody(secondBody)
		, Normal(contactNormal)
		, Overlap(overlap)
		, Position(position)
		, Resolved(false)
	{}

	float SeparatingVelocity() const;

	float TotalImpluse() const;
};

}

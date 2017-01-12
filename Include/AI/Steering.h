#pragma once

#include <Core/Entity.h>
#include <Math/Vector2.h>
#include <Physics/Physics2D.h>

namespace Osm
{

struct SteeringOutput
{
	Vector2	Linear;
	float	Angular;
};

class SteeringBehaviour;

class Steering :	public Component<Entity>,
					public ComponentContainer<Steering>
{
public:
	Steering(Entity& entity);

	SteeringOutput GetSteering();

	PhysicsBody2D* GetPhysicsBody() const { return _physicsBody; }

	float MaxAccelearation = 100.0f;

	float MaxForce = 100.0f;

protected:
	PhysicsBody2D* _physicsBody = nullptr;
};

class SteeringBehaviour : public Component<Steering>
{
public:
	SteeringBehaviour(Steering& steering) : Component(steering) {}

	virtual SteeringOutput GetSteering() = 0;

	PhysicsBody2D& GetPhysicsBody() const;
};

class Seek : public SteeringBehaviour
{
public:
	Seek(Steering& steering) : SteeringBehaviour(steering){}

	virtual SteeringOutput GetSteering() override;

	Vector2 Target;
};

}

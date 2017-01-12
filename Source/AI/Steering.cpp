#include <AI/Steering.h>

using namespace Osm;

Steering::Steering(Entity& entity) : Component(entity)
{
	_physicsBody = entity.GetComponent<PhysicsBody2D>();
}

SteeringOutput Steering::GetSteering()
{
	SteeringOutput so;
	for (auto& c : _components)
	{
		auto s = reinterpret_cast<SteeringBehaviour*>(c.get());
		auto temp = s-> GetSteering();
		so.Linear += temp.Linear;
		so.Angular += temp.Angular;
	}

	return so;
}

PhysicsBody2D& SteeringBehaviour::GetPhysicsBody() const
{
	auto physics = GetOwner().GetPhysicsBody();
	ASSERT(physics);
	return *physics;
}

SteeringOutput Seek::GetSteering()
{
	SteeringOutput so;

	so.Linear = Target - GetPhysicsBody().GetPosition();
	so.Linear.Normalize();
	so.Linear *= GetOwner().MaxAccelearation;

	return so;
}
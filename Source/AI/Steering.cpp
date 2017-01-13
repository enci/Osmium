#include <AI/Steering.h>
#include <Graphics/DebugRenderer.h>

using namespace Osm;

void SteeringOutput::operator+=(SteeringOutput rhs)
{
	Linear += rhs.Linear;
	Angular += rhs.Angular;
}

Steering::Steering(Entity& entity)
	: Component(entity)
	, _flags(STEERING_NONE)
{
	_physicsBody = entity.GetComponent<PhysicsBody2D>();
}

SteeringOutput Steering::GetSteering()
{
	SteeringOutput so;
	if(IsOn(STEERING_SEEK))
	{
		so += Seek(Target);
	}
	if(IsOn(STEERING_ARRIVE))
	{
		so += Arrive(Target, 3.0f);
	}
	if (IsOn(STEERING_OFFSET_PURSUIT))
	{
		so += OffsetPursuit(Agent, Offset);
	}

	return so;
}

SteeringOutput Steering::CalculateWeightedSum()
{
	SteeringOutput so;
	return so;
}

SteeringOutput Steering::CalculatePrioritized()
{
	SteeringOutput so;	
	return so;
}

SteeringOutput Steering::Seek(Vector2& targetPos)
{
	SteeringOutput so;

	so.Linear = targetPos - _physicsBody->GetPosition();
	so.Linear.Normalize();
	so.Linear *= MaxAccelearation;

	return so;
}

SteeringOutput Steering::Arrive(Vector2& targetPos, float deceleration)
{
	Vector2 toTarget = Target - _physicsBody->GetPosition();

	SteeringOutput so;

	// Calculate the distance to the target
	double dist = toTarget.Magnitude();

	if (dist > 0)
	{
		// Because Deceleration is enumerated as an int, this value is required
		// to provide fine tweaking of the deceleration..
		const float DecelerationTweaker = 0.3;

		// Calculate the speed required to reach the target given the desired
		// deceleration
		float speed = dist / ((double)deceleration * DecelerationTweaker);

		// Make sure the velocity does not exceed the max
		speed = min(speed, MaxSpeed);

		// From here proceed just like Seek except we don't need to normalize 
		// the ToTarget vector because we have already gone to the trouble
		// of calculating its length: dist.
		Vector2 desiredVelocity = toTarget * speed / dist;

		so.Linear = (desiredVelocity - _physicsBody->GetVelocity());
	}

	return so;
}

SteeringOutput Steering::OffsetPursuit(const PhysicsBody2D* agent, const Vector2& offset)
{
	if (agent == nullptr)
		return SteeringOutput();

	// Get vehicle coordinate frame
	Vector2 target = agent->GetToWorld(offset);		

	// If the evader is ahead and facing the agent then we can just seek
	// for the evader's current position.
	Vector2 toTarget = target - _physicsBody->GetPosition();

	gDebugRenderer.AddLine(ToVector3(target), ToVector3(_physicsBody->GetPosition()));

	// The lookahead time is propotional to the distance between the evader
	// and the pursuer; and is inversely proportional to the sum of the
	// agent's velocities
	float LookAheadTime = toTarget.Magnitude() /
		(MaxSpeed + agent->GetVelocity().Magnitude());

	// Now seek to the predicted future position of the evader
	target = target + agent->GetVelocity() * LookAheadTime;

	gDebugRenderer.AddCircle(ToVector3(target));

	return Seek(target);
}

/*
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
*/
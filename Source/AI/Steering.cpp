#include <AI/Steering.h>
#include <Graphics/DebugRenderer.h>
#include <imgui.h>
#include <Core/World.h>

using namespace Osm;

Steering::Steering(Entity& entity)
	: Component(entity)
	, _flags(STEERING_NONE)
{
	_physicsManager = GetOwner().GetWorld().GetComponent<PhysicsManager2D>();
	_physicsBody = entity.GetComponent<PhysicsBody2D>();
}

Vector2 Steering::GetSteering()
{
	CalculatePrioritized();
	DebugRender();
	return _current;
}

bool Steering::AccumulateForce(Vector2 add)
{
	// Calculate how much steering force the vehicle has used so far
	float  magnitudeSoFar = _current.Magnitude();

	// Calculate how much steering force remains to be used by this vehicle
	float magnitudeRemaining = MaxForce - magnitudeSoFar;

	// Return false if there is no more force left to use
	if (magnitudeRemaining <= 0.0) return false;

	// Calculate the magnitude of the force we want to add
	double magnitudeToAdd = add.Magnitude();

	// If the magnitude of the sum of ForceToAdd and the running total
	// does not exceed the maximum force available to this vehicle, just
	// add together. Otherwise add as much of the ForceToAdd vector is
	// possible without going over the max.
	if (magnitudeToAdd < magnitudeRemaining)
	{
		_current += add;
	}
	else
	{
		// Add it to the steering force
		add.Normalize();
		_current += add * magnitudeRemaining;
	}

	return true;
}

void Steering::CalculateWeightedSum()
{
}

void Steering::CalculatePrioritized()
{
	_current.Clear();

	Vector2 force;

	vector<PhysicsBody2D*> neighbors;
	if (IsOn(STEERING_SEPARATION) || IsOn(STEERING_ALIGNMENT) || IsOn(STEERING_COHESION))
	{
		Vector2 v = _physicsBody->GetPosition();
		neighbors = GetFlockingNeighbors();
	}

	if (!_physicsManager->IsPhysicsBodyValid(Agent))
		Agent = nullptr;

	if (IsOn(STEERING_SEEK))
	{
		force = Seek(Target) * SeekWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_OFFSET_PURSUIT) && Agent)
	{
		force = OffsetPursuit(Agent, Offset) * OffsetPursuitWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_SEPARATION))
	{
		force = Separation(neighbors) * SeparationWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_COHESION))
	{
		force = Cohesion(neighbors) * CohesionWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_ALIGNMENT))
	{
		force = Alignment(neighbors) * AlignmentWeight;
		if (!AccumulateForce(force)) return;
	}
}

Vector2 Steering::Seek(Vector2& targetPos)
{
	Vector2 seek = targetPos - _physicsBody->GetPosition();
	seek.Normalize();
	seek *= MaxAccelearation;
	return seek;
}

Vector2 Steering::Flee(Vector2& targetPos)
{
	return Vector2();
}

Vector2 Steering::Arrive(Vector2& targetPos, float deceleration)
{
	return Vector2();
}

Vector2 Steering::OffsetPursuit(const PhysicsBody2D* agent, const Vector2& offset)
{
	if (agent == nullptr)
		return Vector2();

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
		(MaxSpeed + agent->GetVelocity().Magnitude()) * 0.1f;

	// Now seek to the predicted future position of the evader
	target = target + agent->GetVelocity() * LookAheadTime;

	gDebugRenderer.AddCircle(ToVector3(target));

	return Seek(target);
}

Vector2 Steering::Evade(const PhysicsBody2D* agent)
{
	return Vector2();
}

Vector2 Steering::Wander()
{
	return Vector2();
}

Vector2 Steering::ObstacleAvoidance()
{
	return Vector2();
}

vector<PhysicsBody2D*> Steering::GetFlockingNeighbors()
{
	vector<PhysicsBody2D*> neighbors;
	Vector2 v = _physicsBody->GetPosition();
	neighbors = _physicsManager->GetInRadius(v, FlockingRadius);
	if (FlockingTag != 0)
	{
		remove_if(	neighbors.begin(), 
					neighbors.end(), 
					[this](PhysicsBody2D* b)
		{
			return b->GetOwner().GetTag() != FlockingTag;
		});
	}
	return neighbors;
}

Vector2 Steering::Cohesion(const std::vector<PhysicsBody2D*>& agents)
{
	Vector2 cohesion;

	// First find the center of mass of all the agents
	Vector2 centerOfMass, steeringForce;

	size_t neighborCount = 0;

	// Iterate through the neighbors and sum up all the position vectors
	for (auto a : agents)
	{
		if (a != _physicsBody && a != Agent)
		{
			centerOfMass += a->GetPosition();
			neighborCount++;
		}
	}

	if (neighborCount > 0)
	{
		// The center of mass is the average of the sum of positions
		centerOfMass *= 1.0f / (float)neighborCount;

		// now seek towards that position
		cohesion = Seek(centerOfMass);
	}

	cohesion.Normalize();

	return cohesion;
}

Vector2 Steering::Separation(const std::vector<PhysicsBody2D*>& agents)
{
	Vector2 steeringForce;

	// iterate through the neighbors and sum up all the position vectors
	for (auto a : agents)
	{
		// make sure this agent isn't included in the calculations and that
		// the agent being examined is close enough
		if (a != _physicsBody && a != Agent)
		{
			Vector2 toAgent = _physicsBody->GetPosition() - a->GetPosition();

			// scale the force inversely proportional to the agents distance  
			// from its neighbor.
			float d = toAgent.Magnitude();
			if (d > 0.01)
			{
				toAgent.Normalize();
				steeringForce += toAgent / d;
			}
		}
	}

	return steeringForce;
}

Vector2 Steering::Alignment(const std::vector<PhysicsBody2D*>& agents)
{
	//This will record the average heading of the neighbors
	Vector2 averageHeading;

	//This count the number of vehicles in the neighborhood
	float neighborCount = 0.0;

	Vector2 currentHeading = _physicsBody->GetForward();
	currentHeading.Normalize();

	//iterate through the neighbors and sum up all the position vectors
	for (auto a : agents)
	{
		// make sure this agent isn't included in the calculations and that
		// the agent being examined is close enough
		if (a != _physicsBody && a != Agent)
		{
			Vector2 heading = a->GetForward();
			heading.Normalize();
			averageHeading += heading;
			neighborCount++;
		}

	}

	// If the neighborhood contained one or more vehicles, average their
	// heading vectors.
	if (neighborCount > 0.0)
	{
		averageHeading *= 1.0f / neighborCount;
		averageHeading -= currentHeading;
	}

	return averageHeading;
}

void Steering::DebugRender()
{
	if (_inspect)
	{
		/*
		auto pos = _physicsBody->GetPosition();
		gDebugRenderer.AddCircle(
			ToVector3(pos),
			_physicsBody->GetRadius(),
			Color::Purple);
		gDebugRenderer.AddLine(
			ToVector3(pos),
			ToVector3(pos + _current),
			Color::Orange);
		gDebugRenderer.AddCircle(
			ToVector3(pos),
			FlockingRadius,
			Color::Purple);
		*/
	}
	_inspect = false;
}

#ifdef INSPECTOR
void Steering::Inspect()
{
	_inspect = true;

	/*
	ImGui::InputFloat("Max Accelearation", &MaxAccelearation);
	ImGui::InputFloat("Max Force", &MaxForce);
	ImGui::InputFloat("Max Speed", &MaxSpeed);
	ImGui::InputFloat("Wander Jitter", &WanderJitter);
	ImGui::InputFloat("Wander Radius", &WanderRadius);
	ImGui::InputFloat("Wander Distance", &WanderDistance);

	float maxed = _current.Magnitude() / MaxSpeed;
	ImGui::ProgressBar(maxed);
	*/
}
#endif
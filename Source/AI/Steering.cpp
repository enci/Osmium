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
	vector<PhysicsBody2D*> neighbors;
	if (IsOn(STEERING_SEPARATION) || IsOn(STEERING_ALIGNMENT) || IsOn(STEERING_COHESION))
	{
		Vector2 v = _physicsBody->GetPosition();
		neighbors = GetFlockingNeighbors();
	}

	Vector2 force;

	if (IsOn(STEERING_SEEK))
	{
		force += Seek(Target) * SeekWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_ARRIVE))
	{
		force += Arrive(Target, ArriveAcceleration)* ArriveWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_EVADE))
	{
		force += Evade(Agent);
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_OFFSET_PURSUIT))
	{
		force += OffsetPursuit(Agent, Offset) * OffsetPursuitWeight;
		if (!AccumulateForce(force)) return;
	}	
	if (IsOn(STEERING_COHESION))
	{
		force += Cohesion(neighbors) * CohesionWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_SEPARATION))
	{
		force += Separation(neighbors) * SeparationWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_ALIGNMENT))
	{
		force += Alignment(neighbors) * AlignmentWeight;
		if (!AccumulateForce(force)) return;
	}
	if (IsOn(STEERING_WANDER))
	{
		force += Wander() * WanderWeight;
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
	/*
	Vector2D DesiredVelocity = Vec2DNormalize(m_pVehicle->Pos() - TargetPos)
		* m_pVehicle->MaxSpeed();
	return (DesiredVelocity - m_pVehicle->Velocity());
	*/

	Vector2 flee = targetPos - _physicsBody->GetPosition();
	flee.Normalize();
	flee *= MaxAccelearation;
	flee = flee - _physicsBody->GetVelocity();
	return flee;
}

Vector2 Steering::Arrive(Vector2& targetPos, float deceleration)
{
	Vector2 toTarget = Target - _physicsBody->GetPosition();

	Vector2 arrive;

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

		arrive = (desiredVelocity - _physicsBody->GetVelocity());
	}

	return arrive;
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
	// Not necessary to include the check for facing direction this time
	Vector2 toPursuer = agent->GetPosition() - _physicsBody->GetPosition();

	//uncomment the following two lines to have Evade only consider pursuers 
	//within a 'threat range'
	if (toPursuer.SquareMagnitude() > EvadeDistance * EvadeDistance)
		return Vector2();

	//the lookahead time is propotional to the distance between the pursuer
	//and the pursuer; and is inversely proportional to the sum of the
	//agents' velocities
	float lookAheadTime = toPursuer.Magnitude() / (MaxSpeed + agent->GetVelocity().Magnitude());
	lookAheadTime *= 0.1f;

	//now flee away from predicted future position of the pursuer
	Vector2 target = agent->GetPosition() + agent->GetVelocity() * lookAheadTime;
	return -1.0f * Flee(target);
}

Vector2 Steering::Wander()
{
	Vector2 wander;

	// This behavior is dependent on the update rate, so this line must
	// be included when using time independent framerate.
	float JitterThisTimeSlice = WanderJitter * 0.016f; // m_pVehicle->TimeElapsed();

	// First, add a small random vector to the target's position
	_wanderTarget += Vector2(	RandInRange(-1.0f, 1.0f) * JitterThisTimeSlice,
								RandInRange(-1.0f, 1.0f) * JitterThisTimeSlice);

	// Reproject this new vector back on to a unit circle
	_wanderTarget.Normalize();

	//increase the length of the vector to the same as the radius
	//of the wander circle
	_wanderTarget *= WanderRadius;

	//move the target into a position WanderDist in front of the agent
	Vector2 target = _wanderTarget + Vector2(0, WanderDistance);

	//project the target into world space
	target = _physicsBody->GetToWorld(target);

	Vector2 center = _physicsBody->GetToWorld(Vector2(0, WanderDistance));

	gDebugRenderer.AddCircle(ToVector3(center), WanderRadius);

	gDebugRenderer.AddCircle(ToVector3(target), 0.5f, Color::Yellow);

	// And steer towards it
	wander = target - _physicsBody->GetPosition();

	return wander;
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
	for(auto a : agents)
	{
		if(a != _physicsBody && a != Agent)
		{
			centerOfMass += a->GetPosition();
			neighborCount++;
		}
	}

	if (neighborCount > 0)
	{
		// The center of mass is the average of the sum of positions
		centerOfMass *= 1.0f / (float)neighborCount;

		//now seek towards that position
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
			Vector2 toAgent =_physicsBody->GetPosition() - a->GetPosition();

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

	Vector2 currentHeading = _physicsBody->GetFroward();
	currentHeading.Normalize();

	//iterate through the neighbors and sum up all the position vectors
	for (auto a : agents)
	{
		// make sure this agent isn't included in the calculations and that
		// the agent being examined is close enough
		if (a != _physicsBody && a != Agent)
		{
			Vector2 heading = a->GetFroward();
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
		auto pos = _physicsBody->GetPosition();
		gDebugRenderer.AddCircle(
			ToVector3(pos),
			_physicsBody->GetRaduis(),
			Color::Purple);
		gDebugRenderer.AddLine(
			ToVector3(pos),
			ToVector3(pos + _current),
			Color::Orange);
		gDebugRenderer.AddCircle(
			ToVector3(pos),
			FlockingRadius,
			Color::Purple);
	}
	_inspect = false;
}

#ifdef INSPECTOR
void Steering::Inspect()
{
	_inspect = true;

	ImGui::InputFloat("Max Accelearation", &MaxAccelearation);
	ImGui::InputFloat("Max Force", &MaxForce);
	ImGui::InputFloat("Max Speed", &MaxSpeed);
	ImGui::InputFloat("Wander Jitter", &WanderJitter);
	ImGui::InputFloat("Wander Radius", &WanderRadius);
	ImGui::InputFloat("Wander Distance", &WanderDistance);

	float maxed = _current.Magnitude() / MaxSpeed;
	ImGui::ProgressBar(maxed);
	// ImGui::SameLine();
	//ImGui::LabelText("Total Used ","");	
}
#endif

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
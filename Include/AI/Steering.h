#pragma once

#include <Core/Entity.h>
#include <Math/Vector2.h>
#include <Physics/Physics2D.h>

namespace Osm
{

enum BehaviorType
{
	STEERING_NONE				= 1 << 0,
	STEERING_SEEK				= 1 << 1,
	STEERING_FLEE				= 1 << 2,
	STEERING_ARRIVE				= 1 << 3,
	STEERING_WANDER				= 1 << 4,
	STEERING_COHESION			= 1 << 5,
	STEERING_SEPARATION			= 1 << 6,
	STEERING_ALIGNMENT			= 1 << 7,
	STEERING_OBSTACLE_AVOIDANCE	= 1 << 8,
	STEERING_WALL_AVOIDANCE		= 1 << 9,
	STEERING_FOLLOW_PATH		= 1 << 10,
	STEERING_PURSUIT			= 1 << 11,
	STEERING_EVADE				= 1 << 12,
	STEERING_INTERPOSE			= 1 << 13,
	STEERING_HIDE				= 1 << 14,
	//STEERING_FLOCK				= 1 << 15, // ? Merge from others
	STEERING_FLOCK				= STEERING_COHESION | STEERING_SEPARATION | STEERING_ALIGNMENT,
	STEERING_OFFSET_PURSUIT		= 1 << 16,
};

class Steering : public Component<Entity>
{
public:
	Steering(Entity& entity);

	Vector2 GetSteering();

	void TurnOn(uint bt)		{ _flags |= bt; }

	void TurnOff(uint bt)		{ _flags ^= bt; }

	bool IsOn(uint bt) const	{ return (_flags & bt) == bt; }

	float MaxAccelearation = 100.0f;

	float MaxForce = 100.0f;

	float MaxSpeed = 10.0f;

	float WanderJitter = 50.0f;

	float WanderRadius = 5.0f;

	float WanderDistance = 15.0f;

	float FlockingRadius = 15.0f;

	float ArriveAcceleration = 3.0f;

	uint FlockingTag = 0;

	Vector2 Target;

	PhysicsBody2D* Agent = nullptr;

	Vector2 Offset;	

	float EvadeDistance = 35.0f;

	// Weights
	float SeekWeight = 1.0f;
	float FleeWeight = 1.0f;
	float ArriveWeight = 1.0f;
	float WanderWeight = 1.0f;
	float CohesionWeight = 1.0f;
	float SeparationWeight = 1.0f;
	float ObstacleAvoidanceWeight = 1.0f;
	float WallAvoidanceWeight = 1.0f;
	float PursuitWeight = 1.0f;
	float EvadeWeight = 1.0f;
	float FoollowPathWeight = 1.0f;
	float HideWeight = 1.0f;
	float OffsetPursuitWeight = 1.0f;
	float AlignmentWeight = 10.0f;

protected:

	bool AccumulateForce(Vector2 addForce);

	// Calculates and sums the steering forces from any active behaviors
	void CalculateWeightedSum();

	// Calculates and sums the steering forces from any active behaviors
	void CalculatePrioritized();

	// This behavior moves the agent towards a target position
	Vector2 Seek(Vector2& targetPos);

	// This behavior returns a vector that moves the agent away
	// from a target position
	Vector2 Flee(Vector2& targetPos);

	// this behavior is similar to seek but it attempts to arrive 
	// at the target position with a zero velocity
	Vector2 Arrive(Vector2& targetPos, float deceleration);

	// This behavior predicts where an agent will be in time T and seeks
	// towards that point to intercept it.
	// SteeringOutput Pursuit(const Vehicle* agent);

	//this behavior maintains a position, in the direction of offset
	//from the target vehicle
	Vector2 OffsetPursuit(const PhysicsBody2D* agent, const Vector2& offset);

	//this behavior attempts to evade a pursuer
	Vector2 Evade(const PhysicsBody2D* agent);

	// This behavior makes the agent wander about randomly
	Vector2 Wander();

	//this returns a steering force which will attempt to keep the agent 
	//away from any obstacles it may encounter
	//SteeringOutput ObstacleAvoidance(const std::vector<BaseGameEntity*>& obstacles);

	//this returns a steering force which will keep the agent away from any
	//walls it may encounter
	//SteeringOutput WallAvoidance(const std::vector<Wall2D> &walls);


	//given a series of Vector2Ds, this method produces a force that will
	//move the agent along the waypoints in order
	//SteeringOutput FollowPath();

	//this results in a steering force that attempts to steer the vehicle
	//to the center of the vector connecting two moving agents.
	// SteeringOutput Interpose(const Vehicle* VehicleA, const Vehicle* VehicleB);

	//given another agent position to hide from and a list of BaseGameEntitys this
	//method attempts to put an obstacle between itself and its opponent
	// SteeringOutput Hide(const Vehicle* hunter, const std::vector<BaseGameEntity*>& obstacles);


	// -- Group Behaviors -- //

	std::vector<PhysicsBody2D*> GetFlockingNeighbors();

	Vector2 Cohesion(const std::vector<PhysicsBody2D*> &agents);

	Vector2 Separation(const std::vector<PhysicsBody2D*> &agents);

	Vector2 Alignment(const std::vector<PhysicsBody2D*> &agents);

	void DebugRender();

#ifdef INSPECTOR
	virtual void Inspect() override;
	bool _inspect = false;
#endif

protected:

	Vector2				_current;
	uint				_flags;
	PhysicsManager2D*	_physicsManager = nullptr;
	PhysicsBody2D*		_physicsBody = nullptr;
	Vector2				_wanderTarget;
};

}

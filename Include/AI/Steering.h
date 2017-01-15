#pragma once

#include <Core/Entity.h>
#include <Math/Vector2.h>
#include <Physics/Physics2D.h>

namespace Osm
{

struct SteeringOutput
{
	Vector2	Linear		= Vector2();
	float	Angular		= 0.0f;
	void operator+=(SteeringOutput rhs);
};

enum BehaviorType
{
	STEERING_NONE				= 1 << 0,
	STEERING_SEEK				= 1 << 1,
	STEERING_FLEE				= 1 << 2,
	STEERING_ARRIVE				= 1 << 3,
	STEERING_WANDER				= 1 << 4,
	STEERING_COHESION			= 1 << 5,
	STEERING_SEPARATION			= 1 << 6,
	STEERING_ALLIGNMENT			= 1 << 7,
	STEERING_OBSTACLE_AVOIDANCE	= 1 << 8,
	STEERING_WALL_AVOIDANCE		= 1 << 9,
	STEERING_FOLLOW_PATH		= 1 << 10,
	STEERING_PURSUIT			= 1 << 11,
	STEERING_EVADE				= 1 << 12,
	STEERING_INTERPOSE			= 1 << 13,
	STEERING_HIDE				= 1 << 14,
	STEERING_FLOCK				= 1 << 15, // ? Merge from others
	STEERING_OFFSET_PURSUIT		= 1 << 16,
};

class Steering :	public Component<Entity>
{
public:
	Steering(Entity& entity);

	SteeringOutput GetSteering();

	void TurnOn(uint bt)		{ _flags |= bt; }

	void TurnOff(uint bt)		{ _flags ^= bt; }

	bool IsOn(uint bt) const	{ return (_flags & bt) == bt; }

	float MaxAccelearation = 100.0f;

	float MaxForce = 100.0f;

	float MaxSpeed = 10.0f;

	float WanderJitter = 50.0f;

	float WanderRadius = 5.0f;

	float WanderDistance = 15.0f;

	Vector2 Target;

	PhysicsBody2D* Agent = nullptr;

	Vector2 Offset;

protected:

	// Calculates and sums the steering forces from any active behaviors
	SteeringOutput CalculateWeightedSum();

	// Calculates and sums the steering forces from any active behaviors
	SteeringOutput CalculatePrioritized();

	// This behavior moves the agent towards a target position
	SteeringOutput Seek(Vector2& targetPos);

	// This behavior returns a vector that moves the agent away
	// from a target position
	// SteeringOutput Flee(Vector2& targetPos);

	// this behavior is similar to seek but it attempts to arrive 
	// at the target position with a zero velocity
	SteeringOutput Arrive(Vector2& targetPos, float deceleration);

	// This behavior predicts where an agent will be in time T and seeks
	// towards that point to intercept it.
	// SteeringOutput Pursuit(const Vehicle* agent);

	//this behavior maintains a position, in the direction of offset
	//from the target vehicle
	SteeringOutput OffsetPursuit(const PhysicsBody2D* agent, const Vector2& offset);

	//this behavior attempts to evade a pursuer
	// SteeringOutput Evade(const Vehicle* agent);

	// This behavior makes the agent wander about randomly
	SteeringOutput Wander();

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

	// SteeringOutput Cohesion(const std::vector<Vehicle*> &agents);
	// SteeringOutput Separation(const std::vector<Vehicle*> &agents);
	// SteeringOutput Alignment(const std::vector<Vehicle*> &agents);	

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

protected:

	uint				_flags;
	PhysicsManager2D*	_physicsManager = nullptr;
	PhysicsBody2D*		_physicsBody = nullptr;
	Vector2				_wanderTarget;
};

}

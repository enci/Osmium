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
	return Vector2();

	CalculatePrioritized();
	DebugRender();
	return _current;
}

bool Steering::AccumulateForce(Vector2 add)
{
	return true;
}

void Steering::CalculateWeightedSum()
{
}

void Steering::CalculatePrioritized()
{
	_current.Clear();
}

Vector2 Steering::Seek(Vector2& targetPos)
{
	return Vector2();
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
	return Vector2();
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
	/*
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
	*/
	return neighbors;
}

Vector2 Steering::Cohesion(const std::vector<PhysicsBody2D*>& agents)
{
	return Vector2();
}

Vector2 Steering::Separation(const std::vector<PhysicsBody2D*>& agents)
{
	return Vector2();
}

Vector2 Steering::Alignment(const std::vector<PhysicsBody2D*>& agents)
{
	return Vector2();
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
#pragma once
#include <Core/Entity.h>
#include <Graphics/MeshRenderer.h>
#include <Physics/Physics2D.h>

class SpaceShip;

class Push : public igad::Entity
{
public:

	Push(igad::World& world);

	virtual ~Push();

	virtual void  Update(float dt)  override;

	virtual void OnCollision(igad::Collision2D& collision, int body) override;

	void Init(SpaceShip* spaceShip);
private:
	igad::Transform* _transform			= nullptr;
	igad::PhysicsBody2D* _physicsBody		= nullptr;
	float _time = 0.0f;
	igad::Vector3 _force;
};


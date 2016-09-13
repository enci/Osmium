#pragma once
#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Graphics/MeshRenderer.h>
#include <Physics/Physics2D.h>
#include "Spaceship.h"

class Projectile : public igad::Entity
{
public:
	Projectile(igad::World& world);

	void Init(SpaceShip* spaceship);

	virtual void Update(float dt) override;

	virtual void OnCollision(igad::Collision2D& collision, int body) override;

private:
	int _player								= -1;
	float _time								= 0.0f;
	igad::Transform* _transform				= nullptr;
	igad::MeshRenderer* _renderer			= nullptr;
	igad::PhysicsBody2D* _physicsBody		= nullptr;
	igad::Light* _light						= nullptr;
};


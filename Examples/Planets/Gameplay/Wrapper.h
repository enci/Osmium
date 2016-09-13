#pragma once

#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Physics/Physics2D.h>
#include "BlasteroidsCamera.h"

class Wrapper : public igad::Component<igad::Entity>
{
public:
	Wrapper(igad::Entity& entity);

	void Update(float dt);

private:
	igad::Vector2 CalculateGameWorldProjection(const igad::Vector2 canonical) const;

	igad::Transform*		_transform	= nullptr;
	igad::PhysicsBody2D*	_body		= nullptr;
	BlasteroidsCamera*		_camera		= nullptr;
	float					_cooldown	= 5.0f;
};

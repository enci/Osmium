#pragma once
#include <Core/Entity.h>
#include <Math/Vector3.h>
#include <Core/Transform.h>
#include <Physics/Physics2D.h>
#include <Graphics/MeshRenderer.h>
#include "Wrapper.h"
#include "Blasteroids.h"

class Ammo : public igad::Entity
{
public:
	Ammo(igad::World& world);
		
	virtual void Update(float dt) override;

	virtual void OnCollision(igad::Collision2D& collision, int body) override;

private:
	
	igad::Transform*		_transform		= nullptr;
	igad::PhysicsBody2D*	_physicsBody	= nullptr;
	igad::MeshRenderer*		_renderer		= nullptr;
	Wrapper*				_wrapper		= nullptr;
};
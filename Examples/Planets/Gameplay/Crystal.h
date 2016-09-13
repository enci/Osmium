#pragma once

#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Physics/Physics2D.h>
#include <Graphics/Render.h>
#include "Wrapper.h"
#include <Graphics/Mesh.h>

class Crystal : public igad::Entity
{
public:
	Crystal(igad::World& world);
	~Crystal();

	void Init(const igad::Vector3& position,
		const igad::Vector2& velocity,
		float size);

	virtual void Update(float dt) override;

	virtual void OnCollision(igad::Collision2D& collision, int body) override;

private:

	void InitCollisionShape();

	igad::Transform*		_transform = nullptr;
	igad::PhysicsBody2D*	_physicsBody = nullptr;
	igad::MeshRenderer*		_renderer = nullptr;
	Wrapper*				_wrapper = nullptr;
	igad::Mesh*				_mesh = nullptr;
	std::vector<igad::Vector2> _collision;
};

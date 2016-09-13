#pragma once

#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Physics/Physics2D.h>
#include <Graphics/Render.h>
#include "Wrapper.h"
#include <Graphics/Mesh.h>

class Crate : public igad::Entity
{
public:
	Crate(igad::World& world);
	~Crate();

	void Init(const igad::Vector3& position, const igad::Vector2& velocity);

	virtual void Update(float dt) override;

	virtual void OnCollision(igad::Collision2D& collision, int body) override;

private:

	//void InitMesh(float radius);

	void InitCollisionShape();

	//void Explode(igad::Entity& other, int amount);

	igad::Transform*		_transform = nullptr;
	igad::PhysicsBody2D*	_physicsBody = nullptr;
	igad::MeshRenderer*		_renderer = nullptr;
	Wrapper*				_wrapper = nullptr;
	igad::Mesh*				_mesh = nullptr;
	std::vector<igad::Vector2> _collision;
};

#pragma once

#include <Core/Entity.h>
#include <Core/Transform.h>
#include <Graphics/Render.h>

/// A simple ray in 3D
struct Ray3D
{
	/// Ray's origin
	igad::Vector3 Origin;

	/// Ray's direction
	igad::Vector3 Direction;

	igad::Vector3 Evaluate(float t) const;
};


class BlasteroidsCamera : public igad::Entity
{
public:

	BlasteroidsCamera(igad::World& world);

	virtual void  Update(float dt)  override;

	void AddShake(float amount) { _shakeIntensity += amount; }

	/// Unproject canonical-space vector
	igad::Vector3 Unproject(const igad::Vector3& v) const;

	/// Unproject canonical-space vector to a ray
	Ray3D Unproject(const igad::Vector2& v) const;

	/*
	/// Unproject screen space vector to a ray
	*/

protected:

	igad::Transform*	_transform;
	igad::Camera*		_camera;
	igad::Vector3		_averagePosition;
	float				_shakeIntensity	= 0.0f;
};

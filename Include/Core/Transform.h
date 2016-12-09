#pragma once

#include <Core/Component.h>
#include <Core/Entity.h>
#include <Math/Matrix44.h>


namespace Osm
{

class Transform : public Component<Entity>
{
public:
	Transform(Entity& entity);

	const Vector3& GetPosition() const					{ return _position; }	

	void SetPosition(const Vector3& pos)				{ _position = pos; }

	const Vector3& GetScale() const						{ return _scale; }

	void SetScale(const Vector3& scale)					{ _scale = scale; }

	void SetUniformScale(float scale)					{ SetScale(Vector3(scale, scale, scale)); }

	const Matrix44& GetOrientation() const				{ return _orientation; }

	void SetOrientation(const Matrix44& orientation)	{ _orientation = orientation; }

	void SetOrientation(const Vector3& x,
						const Vector3& y,
						const Vector3& z);

	Matrix44 GetWorld();

	Matrix44 GetLocal();

	void SetLocal(Matrix44 view);

	void SetParent(Transform* parent)					{ _parent = parent; }

	Transform* GetParent() const						{ return _parent; }

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

protected:

	Transform*	_parent;
	Matrix44	_orientation;
	Vector3		_position;
	Vector3		_scale;
};

inline Transform::Transform(Entity& entity)
	: Component(entity)
	, _parent(nullptr)
	, _orientation()
	, _position(0, 0, 0)
	, _scale(1, 1, 1)
{
}

inline void Transform::SetOrientation(const Vector3& x, const Vector3& y, const Vector3& z)
{
	_orientation.SetOrientation(x, y, z);
}

inline Matrix44 Transform::GetWorld()
{
	Matrix44 world = _orientation * Matrix44::CreateScale(_scale);
	world.SetTranslation(_position);

	if (_parent)
		world = _parent->GetWorld() * world;

	return world;
}

inline Matrix44 Transform::GetLocal()
{
	Matrix44 local = _orientation * Matrix44::CreateScale(_scale);
	local.SetTranslation(_position);
	return local;
}

}



/*

namespace Osm
{

class Transform : public Component<Entity>
{
public:
	Transform(Entity& entity) : Component(entity) { }

	Matrix44& GetTransform() { return _transform; }

	Vector3 GetPosition() { return _transform.GetTranslation(); };

	void SetPosition(const Vector3& pos) { _transform.SetTranslation(pos); }

	Vector3 GetScale();

	void SetScale(Vector3 scale);

	void SetUniformScale(float scale) { SetScale(Vector3(scale, scale, scale)); }

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

protected:

	Matrix44 _transform;
};

}

*/

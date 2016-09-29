#pragma once

#include <Core/Component.h>
#include <Core/Entity.h>
#include <Math/Matrix44.h>

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

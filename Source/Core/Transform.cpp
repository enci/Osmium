#include <Core/Transform.h>

using namespace igad;

Vector3 Transform::GetScale()
{
	float sx = _transform.GetXAxis().Magnitude();
	float sy = _transform.GetYAxis().Magnitude();
	float sz = _transform.GetZAxis().Magnitude();
	return Vector3(sx, sy, sz);
}

void Transform::SetScale(Vector3 scale)
{
	Vector3 currScale = GetScale();
	scale.x /= currScale.x;
	scale.y /= currScale.y;
	scale.z /= currScale.z;
	_transform = _transform * Matrix44::CreateScale(scale);
}
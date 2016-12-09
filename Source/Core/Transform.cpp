#include <Core/Transform.h>
#include <imgui.h>

using namespace Osm;

void Transform::SetLocal(Matrix44 view)
{
	Vector3 x = view.GetXAxis();
	Vector3 y = view.GetYAxis();
	Vector3 z = view.GetZAxis();

	float sx = x.Magnitude();
	float sy = y.Magnitude();
	float sz = z.Magnitude();

	x *= 1.0f / sx;
	y *= 1.0f / sy;
	z *= 1.0f / sz;

	_scale = Vector3(sx, sy, sz);
	_position = view.GetTranslation();
	_orientation.SetOrientation(x, y, z);
}

void Transform::Inspect()
{
	//ImGui::
}

#include <Core/Transform.h>
#include <imgui.h>
#include <Core/Entity.h>
#include <Core/World.h>
#include <algorithm>

using namespace Osm;

Transform::~Transform()
{
	// Remove from parent
	if (_parent)
	{
		_parent->_childern.erase(
			remove(_parent->_childern.begin(), _parent->_childern.end(), this),
			_parent->_childern.end());
	}

	// Delete childern
	for (auto t : _childern)
	{
		t->GetOwner().GetWorld().RemoveEntity(&t->GetOwner());
		t->_parent = nullptr;
	}
}

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

void Transform::SetParent(Transform* parent)
{
	_parent = parent;
	_parent->_childern.push_back(this);
}

#ifdef INSPECTOR
void Transform::Inspect()
{
	ImGui::DragFloat3("Position", _position.f);
	ImGui::DragFloat3("Scale", _scale.f);

	Vector3 euler = _orientation.GetEulerAngles();
	ImGui::DragFloat3("Rotation", euler.f, 0.001f);
	_orientation.SetEulerAngles(euler);
}
#endif


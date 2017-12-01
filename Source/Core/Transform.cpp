#include <Core/Transform.h>
#include <imgui.h>
#include <Core/Entity.h>
#include <Core/World.h>
#include <algorithm>
#include <ImGuizmo/ImGuizmo.h>
#include "Graphics/Render.h"

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

void Transform::SetParent(Transform* parent, ParentType type)
{
	_parent = parent;
	_parent->_childern.push_back(this);
	_parentType = type;
}

#ifdef INSPECTOR
void Transform::Inspect()
{
	ImGui::DragFloat3("Position", _position.f);
	ImGui::DragFloat3("Scale", _scale.f);
	Vector3 euler = _orientation.GetEulerAngles();
	ImGui::DragFloat3("Rotation", euler.f, 0.001f);
	//_orientation.SetEulerAngles(euler);


	auto camera = GetOwner().GetWorld().GetComponent<RenderManager>()->GetCamera();
	//Matrix44 world = GetWorld();
	Matrix44 view = camera->GetView();
	Matrix44 proj = camera->Projection();

	//float s = 30.0f;
	//world = world * Matrix44::CreateScale(Vector3(s, s, s));
	
	ImGuizmo::BeginFrame();

	/*
	 *
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix.m16, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.m16);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	//vec_t snap;
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	// ImGuizmo::Manipulate();
	// ImGuizmo::Manipulate(camera.mView.m16, camera.mProjection.m16, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m16, NULL, useSnap ? &snap.x : NULL);
	*/


	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;


	
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	
	float matrix[16];
	euler *= 180.0f / Pi;
	ImGuizmo::RecomposeMatrixFromComponents(_position.f, euler.f, _scale.f, matrix);
	ImGuizmo::Manipulate(view.f, proj.f, mCurrentGizmoOperation, ImGuizmo::WORLD, matrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);

	_position.x = matrixTranslation[0];
	_position.y = matrixTranslation[1];
	_position.z = matrixTranslation[2];

	_scale.x = matrixScale[0];
	_scale.y = matrixScale[1];
	_scale.z = matrixScale[2];

	matrixRotation[0] *= Pi / 180.0f;
	matrixRotation[1] *= Pi / 180.0f;
	matrixRotation[2] *= Pi / 180.0f;

	_orientation.SetEulerAngles(Vector3(
			matrixRotation[0],
			matrixRotation[1],
			matrixRotation[2]));	


	ImGui::InputFloat3("Rt", matrixRotation, 3);
}
#endif


#include <Core/Entity.h>
#include <imgui.h>

using namespace Osm;

uint Entity::_nextValidID = 0;

Entity::Entity(World& world)
	: _ID(GetNextValidID())
	, _name("")
	, _world(world)	
{}

void Entity::Inspect()
{
	ImGui::LabelText("Name:", _name.c_str());
	ImGui::LabelText("ID:", std::to_string(_ID).c_str());
	for (auto& c : _components)
		c->Inspect();
}
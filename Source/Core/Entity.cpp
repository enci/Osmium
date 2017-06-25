#include <Core/Entity.h>
#include <Utils.h>
#include <imgui.h>

using namespace Osm;
using namespace std;

uint Entity::_nextValidID = 0;

Entity::Entity(World& world)
	: _ID(GetNextValidID())
	, _name("")
	, _world(world)	
{}

void Entity::Inspect()
{
	for (auto& c : _components)
	{
		string name = typeid(*c).name();
		name = StringReplace(name, "class ", "");

		ImGui::PushID(c.get());
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			c->Inspect();
		}
		ImGui::PopID();
	}
}
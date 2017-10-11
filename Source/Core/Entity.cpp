#include <Core/Entity.h>
#include <Utils.h>
#include <imgui.h>

using namespace Osm;
using namespace std;

// uint Entity::_nextValidID = 0;

Entity::Entity() : _name("") {}

#ifdef INSPECTOR
void Entity::Inspect()
{
	for (auto& c : _components)
	{
		string name = typeid(*c).name();
		name = StringReplace(name, "class ", "");

		ImGui::PushID(c.get());
		if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			c->Inspect();
		}
		ImGui::PopID();
	}
}

#endif
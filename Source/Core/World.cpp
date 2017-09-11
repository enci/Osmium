#include <Core/World.h>
#include <Core/Entity.h>
#include <Core/Transform.h>
#include <imgui.h>
#include <algorithm>
#include <Utils.h>

using namespace std;
using namespace Osm;

World::~World()
{
}

void Osm::World::RemoveEntity(Entity * e)
{
	_removeQueue.insert(e);
}

void World::Update(float dt)
{
	// Add entities
	for (auto& e : _addQueue)
		_entities.push_back(move(e));
	_addQueue.clear();

	// Update entites
	for (auto& e : _entities)
		e->Update(dt);

	while (_removeQueue.size() > 0)
	{
		Entity* r = *_removeQueue.begin();
		_removeQueue.erase(_removeQueue.begin());
		auto toRemove = remove_if(_entities.begin(), _entities.end(), [this, r](unique_ptr<Entity>& entity)
		{
			return r == entity.get();
		});
		_entities.erase(toRemove, _entities.end());
	}
}

void World::PostUpdate(float dt)
{
	// Update entites
	for (auto& e : _entities)
		e->PostUpdate(dt);
}

void World::Render()
{
}

void Osm::World::Clear()
{
	_entities.clear();
	_addQueue.clear();
	_removeQueue.clear();
}


Entity* World::GetEntityByID(uint id)
{
	for (auto& e : _entities)
	{
		if (e->GetID() == id)
			return e.get();
	}
	return nullptr;
}

#ifdef INSPECTOR

void World::InspectEntity(Entity* entity, set<Entity*>& inspected, uint& selected)
{ 
	if(inspected.find(entity) != inspected.end())
		return;	

	static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	string name = entity->GetName();
	uint id = entity->GetID();
	auto trn = entity->GetComponent<Transform>();

	if (name.empty())
		name = typeid(*entity).name();
	name = StringReplace(name, "class ", "");
	// name += " - ID:" + to_string();

	if (trn && trn->GetChildern().size() > 0)
	{		
		auto childern = trn->GetChildern();


		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, name.c_str());
		if (ImGui::IsItemClicked())
		{
			selected = id;
			_entityInspect = true;
		}
		if (nodeOpen)
		{
			for (auto child : childern)
			{
				auto e = &child->GetOwner();
				InspectEntity(e, inspected, selected);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::TreeNodeEx(
			(void*)(intptr_t)id,
			node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen,
			name.c_str());
		if (ImGui::IsItemClicked())
		{
			selected = id;
			_entityInspect = true;
		}
	}
}

void World::Inspect()
{
	for (auto& c : _components)
	{
		string name = typeid(*c).name();
		name = StringReplace(name, "class ", "");
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			c->Inspect();
		}
	}


	ImGui::Text("Entities");
	ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 2.0f);
	ImGui::BeginChild("Entities Window", ImVec2(0, 0), true);

	// set<Entity*> inspected;
	// vector<bool> selected(_entities.size(), false);
	// static uint Id = -1;
	static uint selectedId = -1;

	
	set<Entity*> inspected;
	for (size_t i = 0; i < _entities.size(); i++)
	{
		InspectEntity(_entities[i].get(), inspected, selectedId);
	}



	/*
	for (size_t i = 0; i < _entities.size(); i++)
	{
		auto& e = _entities[i];
		string name = e->GetName();
		if (name.empty())
			name = typeid(*e).name();
		name = StringReplace(name, "class ", "");
		name += " - ID:" + to_string(e->GetID());

		if (ImGui::Selectable(name.c_str()))
		{
			selectedId = e->GetID();
		}
	}
	*/

	if (selectedId != -1)
	{
		Entity* e = GetEntityByID(selectedId);
		if (e)
		{
			ImGui::Begin("Entity Inspector", &_entityInspect);
			ImGui::Text("Name: %s", e->GetName().c_str());
			ImGui::Text("ID: %d", e->GetID());
			ImGui::Separator();
			e->Inspect();
			ImGui::End();
		}

		if (!_entityInspect)
			selectedId = -1;
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
}
#endif
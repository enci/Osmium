#include <Core/World.h>
#include <Core/Entity.h>
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

	// Goes over all the entities and deletes the ones for which the predicate is true.
	auto toRemove = remove_if(_entities.begin(), _entities.end(), [this](unique_ptr<Entity>& entity)
	{
		// Check if this entity is on the remove queue. If so delete and remove from
		// queue when there.
		for (auto itr = _removeQueue.begin(); itr != _removeQueue.end(); ++itr)
		{
			if (*itr == entity.get())
			{
				_removeQueue.erase(itr);
				return true;
			}
		}
		return false;
	});
	_entities.erase(toRemove, _entities.end());
}

void World::PostUpdate(float dt)
{
	// Update entites
	for (auto& e : _entities)
		e->PostUpdate(dt);
}

void Osm::World::Clear()
{
	_entities.clear();
	_addQueue.clear();
	_removeQueue.clear();
}

#ifdef INSPECTOR
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
	ImGui::Separator();
	for (auto& e : _entities)
	{
		string name = e->GetName();
		if (name.empty())
			name = typeid(*e).name();
		name = StringReplace(name, "class ", "");
		name += " - ID:" + to_string(e->GetID());

		if (ImGui::TreeNode(name.c_str()))
		{
			ImGui::TreePop();
		}
	}
}
#endif
#include <Core/World.h>
#include <Core/Entity.h>
#include <algorithm>

using namespace std;
using namespace igad;

World::~World()
{
}

void igad::World::RemoveEntity(Entity * e)
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

void igad::World::Clear()
{
	_entities.clear();
	_addQueue.clear();
	_removeQueue.clear();
}

/*
////////////////////////////////////////////////////////////////////////////////
// Queues adding an entity
////////////////////////////////////////////////////////////////////////////////
template<class T>
void EntityContainer<T>::RemoveEntity(T* e)
{
	// No double removes
	for (auto entity : removeQueue)
		if (entity == e)
			return;
	//
	removeQueue.insert(e);

	// Let the other entities know
	// EntityDeletedMessage msg(*e);
	// BroadcastMessage(msg);
}


////////////////////////////////////////////////////////////////////////////////
// GetEntityByID
////////////////////////////////////////////////////////////////////////////////
template<class T>
T* EntityContainer<T>::GetEntityByID(int id)
{
	for (auto entity : entities)
		if (entity->GetID() == id)
			return entity;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// GetEntityByName
////////////////////////////////////////////////////////////////////////////////
template<class T>
T* EntityContainer<T>::GetEntityByName(std::string name)
{
	for (auto entity : entities)
		if (entity->Name().compare(name) == 0)
			return entity;

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Select entities
////////////////////////////////////////////////////////////////////////////////
template<class T>
std::vector<T*> EntityContainer<T>::SelectEntities(std::function<bool(T*)> filter)
{
	std::vector<T*> selection;
	for (auto entity : entities)
		if (filter(entity))
			selection.push_back(entity);

	return selection;
}

////////////////////////////////////////////////////////////////////////////////
// Select and cast entities
////////////////////////////////////////////////////////////////////////////////
template<class T>
template<class C>
std::vector<C*> EntityContainer<T>::SelectAndCastEntities(std::function<bool(T*)> filter)
{
	std::vector<C*> selection;
	for (auto entity : entities)
		if (filter(entity))
			selection.push_back(static_cast<C*>(entity));

	return selection;
}

////////////////////////////////////////////////////////////////////////////////
// Update
////////////////////////////////////////////////////////////////////////////////
template<class T>
void EntityContainer<T>::Update(float dt)
{
	// Add entities
	for (auto e : addQueue)
	{
		entities.push_back(e);
		e->Added();
	}
	addQueue.clear();

	// Update entites
	for (auto entity : entities)
		entity->Update(dt);

	// Remove after update so that no new entities have been added
	for (auto entity : removeQueue)
	{
		entities.remove(entity);        // Derefence j as it holds an pointer
		SafeDelete(entity);             // THIS is (not) a solution
	}
	removeQueue.clear();
}


////////////////////////////////////////////////////////////////////////////////
// Commit
////////////////////////////////////////////////////////////////////////////////
template<class T>
void EntityContainer<T>::Commit()
{
	// Add entities
	for (auto e : addQueue)
	{
		entities.push_back(e);
		e->Added();
	}
	addQueue.clear();

	// Remove after update so that no new entities have been added
	for (auto entity : removeQueue)
	{
		entities.remove(entity);        // Derefence j as it holds an pointer
		SafeDelete(entity);             // THIS is (not) a solution
	}
	removeQueue.clear();
}


////////////////////////////////////////////////////////////////////////////////
// Clear
////////////////////////////////////////////////////////////////////////////////
template<class T>
void EntityContainer<T>::Clear()
{
	// Clear current entities
	for (auto e : entities)
		SafeDelete(e);
	entities.clear();

	// Clear enities that were to be added
	for (auto e : addQueue)
		SafeDelete(e);
	addQueue.clear();

	// Clear remove queue
	for (auto e : removeQueue)
		SafeDelete(e);
	removeQueue.clear();

	//Entity::ResetNextValidID();
}
*/
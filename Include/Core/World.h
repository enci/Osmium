#pragma once
#include <list>
#include <set>
#include <vector>
#include <memory>
#include <Core/Entity.h>

namespace Osm
{
class Entity;

class World : public ComponentContainer<World>
{
protected:

	typedef std::vector<std::unique_ptr<Entity>>	EntityInnerContainer;		// Owns during execution
	typedef std::set<Entity*>						EntityRemoveQueue;			// Doesn't own anything
	typedef std::vector<std::unique_ptr<Entity>>	EntityAddQueue;				// Owns them till transfered to the inner container 

public:
	/// Ctor
	World() {}

	/// Dtor
	virtual ~World();

	/// Adds and entity to the collection
	template<class T>
	T* CreateEntity();

	/// Removes an entity from this collection
	void RemoveEntity(Entity* e);

	/// Updates the entities
	virtual void Update(float dt);

	/// Updates the entities
	virtual void PostUpdate(float dt);

	/// Generic rendering method (if the rendered is moved completely
	/// into the engine, this can go)
	virtual void Render() {}

	// TODO: Remove maybe
	virtual bool IsDone() { return false; }

	/// Removes and deletes all entities in this container
	void Clear();

	/// Get the first entity of a certain type
	template<class T>
	std::vector<T*> GetEntitiesByType();

#ifdef INSPECTOR
	virtual void Inspect();
#endif

protected:

	/// All the entities in this world
	EntityInnerContainer            _entities;

	/// Queue so that adding can be done form the game loop itself
	EntityAddQueue                  _addQueue;

	/// Queue so that removing can be done form the game loop itself
	EntityRemoveQueue               _removeQueue;
};

template<class T>
inline T* World::CreateEntity()
{
	T* entity = new T(*this);
	_addQueue.push_back(std::unique_ptr<T>(entity));
	return entity;
}

template <class T>
inline	std::vector<T*> World::GetEntitiesByType()
{
	std::vector<T*> selection;
	for (auto& e : _entities)
	{
		T* found = dynamic_cast<T*>(e.get());
		if (found)
			selection.push_back(found);
	}
	return selection;
}

}

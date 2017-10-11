#pragma once

#include <string>
#include <Defines.h>
#include <Core/Component.h>

namespace Osm
{

struct Collision2D;
class World;

///
/// Entity 
///
class Entity : public ComponentContainer<Entity>
{
public:
	friend class World;

public:
	/// New entity with a world
	Entity();

	/// Can't copy entities
	Entity(Entity& other) = delete;

	/// Dtor
	virtual ~Entity() {}

	/// For gameplay code
	virtual void        Update(float dt)	{};

	/// For gameplay code
	virtual void        PostUpdate(float dt) {};

	/// For gameplay code
	virtual void        OnCollision(Collision2D& collision, int body) {}

#ifdef INSPECTOR
	virtual void		Inspect();
#endif

	/// Get the id of this entity
	uint                GetID() const		{ return _ID; }

	/// Get the name of this entity
	const std::string&  GetName() const		{ return _name; }

	/// Set the name of this entity
	void SetName(const std::string& name)	{ _name = name; }

	/// Get the world this entity lives in
	World& GetWorld() const { _ASSERT(_world); return *_world; }

	/// Get a generic tag
	uint GetTag() const					{ return _tag; }

	/// Set a generic tag
	void SetTag(uint tag)				{ _tag = tag; }

private:

	/// Use this to grab the next valid ID
	// static uint         GetNextValidID() { return ++_nextValidID; }

	/// This can be used to reset the next ID (beginning of a level or such)
	// static void         ResetNextValidID() { _nextValidID = 1; }

	//protected:

private:

	/// Each entity has a unique ID (set by World). 0 is not a valid ID
	uint				_ID		= 0;

	/// A name for the entity, this helps when debugging
	std::string			_name;	

	/// Reference to the world  (set by World)
	World*				_world	= nullptr;

	/// This is the next valid ID. Each time a Entity2D is instantiated
	/// this value is updated
	// static uint			_nextValidID;

	/// Just a generic tag
	uint				_tag = 0;
};

}

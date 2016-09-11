#pragma once

#include <string>
#include <Defines.h>
#include <Core/Component.h>

namespace igad
{

struct Collision2D;
class World;

///
/// Entity 
///
class Entity : public ComponentContainer<Entity>
{
public:
	/// New entity with a world
	Entity(World& world);

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

	/// Get the id of this entity
	uint                GetID() const		{ return _ID; }

	/// Get the name of this entity
	const std::string&  GetName() const		{ return _name; }

	/// Set the name of this entity
	void SetName(const std::string& name)	{ _name = name; }

	/// Get the world this entity lives in
	World& GetWorld() const					{ return _world; };

private:

	/// Use this to grab the next valid ID
	static uint         GetNextValidID() { return ++_nextValidID; }

	/// This can be used to reset the next ID (beginning of a level or such)
	static void         ResetNextValidID() { _nextValidID = 1; }

private:

	/// Each entity has a unique ID
	uint				_ID;

	/// A name for the entity, this helps when debugging
	std::string			_name;	

	/// Reference to the world
	World&				_world;

	/// This is the next valid ID. Each time a Entity2D is instantiated
	/// this value is updated
	static uint			_nextValidID;
};

}

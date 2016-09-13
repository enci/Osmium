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
	virtual void        OnCollision(Collision2D& collision, int body) {}

	/// Get the name of this entity
	const std::string&  GetName() const		{ return _name; }

	/// Set the name of this entity
	void SetName(const std::string& name)	{ _name = name; }

	/// Get the world this entity lives in
	World& GetWorld() const					{ return _world; };

private:

	/// A name for the entity, this helps when debugging
	std::string			_name;	

	/// Reference to the world
	World&				_world;
};

}

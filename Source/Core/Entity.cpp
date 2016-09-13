#include <Core/Entity.h>

using namespace igad;

Entity::Entity(World& world)
	: _name("")
	, _world(world)	
{}

//void Entity::OnContact(Collision& collision, Entity& other) {}

//void Entity::OnCollision(Collision2D& collision, int body) {}
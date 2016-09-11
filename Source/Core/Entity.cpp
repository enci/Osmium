#include "stdafx.h"
#include "Entity.h"

using namespace igad;

uint Entity::_nextValidID = 0;

Entity::Entity(World& world)
	: _ID(GetNextValidID())
	, _name("")
	, _world(world)	
{}

//void Entity::OnContact(Collision& collision, Entity& other) {}

//void Entity::OnCollision(Collision2D& collision, int body) {}
#pragma once

#include <Core/Component.h>
#include <Core/Entity.h>
#include <selene/selene.h>
#include <Physics/Physics2D.h>

namespace Osm
{

class Script : public Component<Entity>
{
public:
	Script(Entity& entity);

	void  Update(float dt) {};

	void Init(const std::string& scriptFilename);

	PhysicsBody2D* GetPhysicsBody2D();

protected:

	sel::Selector* _init = nullptr;

	sel::Selector* _update = nullptr;	

	std::string		_filename;
};

class ScriptManager : public Component<World>
{
public:
	ScriptManager(World& world);

protected:
	sel::State _lua;

};

}
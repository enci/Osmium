#include <Core/Scripting.h>
#include <Core/World.h>

using namespace Osm;
using namespace sel;

Script::Script(Entity& entity): Component(entity)
{
}

void Script::Init(const std::string& scriptFilename)
{
	_filename = scriptFilename;
	auto scriptManager = GetEntity().GetWorld().GetComponent<ScriptManager>();	
}

Osm::ScriptManager::ScriptManager(World& world) : Component(world)
{


	_lua["Entity"].SetClass<Entity, uint>("Get", &Entity::GetID);

	_lua["world"].SetObj(world, "RemoveEntity", &World::RemoveEntity);

	//_lua.
	//_lua["Entity"].SetClass<>()
}
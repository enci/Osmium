#include <Core/Scripting.h>
#include <Core/World.h>

using namespace Osm;
using namespace sel;
using namespace std;

Script::Script(Entity& entity): Component(entity)
{
}

void Script::Init(const std::string& scriptFilename, const std::string& name)
{
	_filename = scriptFilename;
	auto scriptManager = GetEntity().GetWorld().GetComponent<ScriptManager>();
	scriptManager->Load(scriptFilename, name);
}

Osm::ScriptManager::ScriptManager(World& world)
	: Component(world)
	, _lua(true)
{
	_lua.HandleExceptionsPrintingToStdOut();
	bool s = _lua("print(\"Lua scripting init. \")");
	ASSERT(s);

	// Register stuff
	_lua["Entity"].SetClass<Entity, World&>("Get", &Entity::GetID);

	_lua["world"].SetObj(world, "RemoveEntity", &World::RemoveEntity);
}

void ScriptManager::Load(const std::string& scriptFilename, const std::string& name)
{
	ullong id = StringHash(scriptFilename);
	if (_loaded.find(id) == _loaded.end())
	{
		_lua.Load(scriptFilename);
		_loaded.insert(id);
	}

	string createFunc = "Create" + name;
	auto obj = _lua[createFunc.c_str()]();
	obj["init"]();
	_scriptObjects.push_back(obj);
}

void ScriptManager::Update(float dt)
{
	for (size_t i = 0; i < _scriptObjects.size(); i++)
	{
		_scriptObjects[i]["update"](dt);
	}
}
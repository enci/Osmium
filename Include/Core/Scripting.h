#pragma once

#include <Core/Component.h>
#include <Core/Entity.h>
#include <selene/selene.h>
#include <Physics/Physics2D.h>
#include <set>

namespace Osm
{

class ScriptManager;

class Script : public Component<Entity>
{
	friend class ScriptManager;

public:
	Script(Entity& entity);

	void Init(const std::string& scriptFilename, const std::string& name);

protected:

	std::string		_filename;
};

class ScriptManager : public Component<World>
{
public:
	ScriptManager(World& world);

	void Load(const std::string& scriptFilename, const std::string& name);

	void Update(float dt);

protected:
	sel::State					_lua;

	std::set<uint>				_loaded;

	std::vector<sel::Selector>	_scriptObjects;
};

}
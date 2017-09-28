#pragma once
#include <Core/Component.h>
#include <Core/Resource.h>
#include <Core/Game.h>
#include <unordered_map>

namespace Osm
{

class ResourceManager : public Component<CGame>
{
public:
	ResourceManager(CGame& engine) : Component(engine) {}

	~ResourceManager() { /* TODO: Make sure there are no resources */ }

	template<typename T, typename... Args>
	T* LoadResource(Args... args);

	template<typename T, typename... Args>
	T* CreateResource(Args... args);

	void ReleaseResource(Resource* res);

	void Update(float deltaTime);

#ifdef INSPECTOR
	void Inspect() override;
	uint FilterFlags = 0xFFFFFFFF;
	bool ShowGenerated = false;
#endif

protected:

	template<typename T>
	T* GetLoadedResource(ullong id);

	/// Where the resources are
	std::string										_resourcesPath;

	/// A map of resources
	std::unordered_map<ullong, Resource*>			_resources;

	/// A map of reference counters
	std::unordered_map<ullong, uint>				_refCounters;
};

template <typename T, typename ... Args>
T* ResourceManager::CreateResource(Args ... args)
{
	T* resource = new T(args...);
	ullong id = T::CalculateResourceID(resource->Path());
	_resources.insert(std::make_pair(id, resource));
	_refCounters.insert(std::make_pair(id, 1));
	resource->_resourceID = id;
	resource->_generated = true;
	return resource;
}

template<typename T, typename... Args>
T* ResourceManager::LoadResource(Args... args)
{	
	ullong id = T::CalculateResourceID(args...);

	T* resource = GetLoadedResource<T>(id);
	if(resource)
		return resource;

	resource = new T(args...);
	_resources.insert(std::make_pair(id, resource));
	_refCounters.insert(std::make_pair(id, 1));
	resource->_resourceID = id;
	return resource;
}

template <typename T>
T* ResourceManager::GetLoadedResource(ullong id)
{
	auto it = _resources.find(id);

	// Check cache
	if (it != _resources.end())
	{
		++_refCounters[id];
		return dynamic_cast<T*>(it->second);
	}

	return nullptr;
}

}

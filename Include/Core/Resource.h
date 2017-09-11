#pragma once

#include <string>
#include <map>
#include <functional>
#include <Utils.h>

namespace Osm
{

/// All supported resource types
enum ResourceType
{
	RESOURCE_TYPE_TEXTURE		= 1 << 0,
	RESOURCE_TYPE_SOUND			= 1 << 1,
	RESOURCE_TYPE_XML			= 1 << 2,
	RESOURCE_TYPE_LABEL			= 1 << 3,
	RESOURCE_TYPE_SHADER		= 1 << 4,
	RESOURCE_TYPE_FONT			= 1 << 5,
	RESOURCE_TYPE_RESOURCEPACK	= 1 << 6,
	RESOURCE_TYPE_SVG			= 1 << 7,
	RESOURCE_TYPE_MESH			= 1 << 8,
};

class ResourceManager;

///
/// Base class to all resources, gives book keeping option to the manager
///
class Resource
{
	friend class ResourceManager;

public:

	/// Get the id for times where is better to use it instead of a
	/// pointer to the resource.
	///
	ullong ResourceID() const { return _resourceID; }

	/// Retrieve the type of this resource, as set by constructor.
	///
	/// @returns the resource type.
	///
	ResourceType Type() const { return _type; }

	/// Returns the path to this resource.
	///
	/// @returns the resource path.
	///
	const std::string& Path() const { return _resourcePath; }
	

	/// Returns true if the resource has been generated, rather than loaded
	///
	bool Generated() const { return _generated; }

	/// A default way to create a resource ID. Resources that have more than a single parameter
	/// (shader for example) can overload the method. The Resources class will no complie if
	/// there is no match.
	static ullong CalculateResourceID(std::string path) { return Osm::StringHash(path); }

protected:

	/// Protected ctor, as resources are handled by the ResourceManager.
	///
	/// @param type the resource type of this resource.
	///
	Resource(ResourceType type) : _resourceID(0), _size(0), _type(type) { }

	/// Protected dtor, as resources are handled by the ResourceManager.
	virtual ~Resource() {}

	/// Reload, not madatory to handle this by all resources at current.
	virtual void Reload() {}

protected:

	/// This is how the manager tracks the resources
	ullong						_resourceID;

	/// Saves the path for runtime reloading of assets
	std::string                 _resourcePath;

	/// Size in memory (not on disk) in bytes
	unsigned int                _size;

	/// The type of this resource, as set by the constructor.
	ResourceType                _type;

	/// True if the resource has been generated, rather than loaded
	bool						_generated = false;

#ifdef INSPECTOR
public:
	bool	Reloaded			= false;
protected:
	bool	QueueReload			= false;
	bool	AutoReload			= false;
	float	ReloadTimer			= 0.0f;
#endif
};

}

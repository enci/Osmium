#include <Core/Resources.h>
#include <Defines.h>

using namespace Osm;

void ResourceManager::ReleaseResource(Resource* res)
{
	// Get id
	auto id = res->ResourceID();

	// Get the number of references to it
	auto refIter = _refCounters.find(id);
	if (refIter != _refCounters.end())
	{
		refIter->second--;
		if (refIter->second == 0)
		{
			auto resItr = _resources.find(id);
			ASSERT(resItr != _resources.end());
			_refCounters.erase(refIter);
			_resources.erase(resItr);
		}		
	}
}
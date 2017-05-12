#include <Core/Resources.h>
#include <Defines.h>
#include <imgui/imgui.h>


const float kAutoReloadTime = 2.0f;

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

void ResourceManager::Update(float dt)
{
#ifdef INSPECTOR
	for (auto& itr : _resources)
	{
		auto r = itr.second;

		// Next frame they are no longer freshly reload
		if (r->Reloaded)
			r->Reloaded = false;

		if (r->QueueReload)
		{
			r->Reload();
			r->Reloaded = true;
			r->QueueReload = false;
		}

		if(r->AutoReload)
		{
			r->ReloadTimer += dt;
			if(r->ReloadTimer > kAutoReloadTime)
			{
				r->Reload();
				r->Reloaded = true;
				r->ReloadTimer = 0.0f;
			}
		}
		else
		{
			r->ReloadTimer = 0.0f;
		}
	}
#endif
}

#ifdef INSPECTOR
void ResourceManager::Inspect()
{
	int i = 0;
	for(auto& itr : _resources)
	{
		ImGui::PushID(i++);
		auto r = itr.second;
		if (ImGui::Button("Reload"))
		{
			r->QueueReload = true;
		}
		ImGui::SameLine();
		ImGui::Checkbox("Sync", &r->AutoReload);
		ImGui::SameLine();
		ImGui::Text(r->Path().c_str());
		ImGui::Separator();
		ImGui::PopID();
	}
}
#endif


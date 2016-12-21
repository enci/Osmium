#include <Audio/Audio.h>
#include <fmod_studio.hpp>
#include <Defines.h>

using namespace Osm;

int ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		LOG("[FMOD] ERROR: %d ", result);
		return 1;
	}
	return 0;
}

AudioManager::AudioManager(CEngine& owner) : Component(owner)
{
	FMOD::Studio::System* mpStudioSystem;
	ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
}
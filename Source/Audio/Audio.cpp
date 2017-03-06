#include <Audio/Audio.h>
#include <fmod_studio.hpp>
#include <Defines.h>

using namespace Osm;
using namespace std;

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
	ErrorCheck(FMOD::Studio::System::create(&_studioSystem));
	ErrorCheck(	_studioSystem->initialize(32,
				FMOD_STUDIO_INIT_LIVEUPDATE,
				FMOD_INIT_PROFILE_ENABLE,
				nullptr));

	ErrorCheck(_studioSystem->getLowLevelSystem(&_system));
}

AudioManager::~AudioManager()
{
	ErrorCheck(_studioSystem->unloadAll());
	ErrorCheck(_studioSystem->release());
}

void AudioManager::Update(float dt)
{
	// Remove inactive channels
	for (auto it = _channels.begin(); it != _channels.end();)
	{
		bool isPlaying = false;
		it->second->isPlaying(&isPlaying);
		if (!isPlaying)
			it = _channels.erase(it);
		else
			++it;
	}
	
	ErrorCheck(_studioSystem->update());
}

void AudioManager::LoadBank(const std::string& bankName,
							FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	auto foundIt = _banks.find(bankName);
	if (foundIt != _banks.end())
		return;

	FMOD::Studio::Bank* bank;
	ErrorCheck(_studioSystem->loadBankFile(bankName.c_str(), flags, &bank));
	if (bank) 		
		_banks[bankName] = bank;
}

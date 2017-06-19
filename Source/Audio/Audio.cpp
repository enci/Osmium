#include <Audio/Audio.h>
#include <fmod_studio.hpp>
#include <Defines.h>
#include <fmod_errors.h>

using namespace Osm;
using namespace std;

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line)
{
	if (result != FMOD_OK)
	{
		LOG("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}

AudioManager::AudioManager(CGame& owner) : Component(owner)
{	
	ERRCHECK(FMOD::Studio::System::create(&_studioSystem));

	ERRCHECK(_studioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));

	//ERRCHECK(	_studioSystem->initialize(1028,
	//			FMOD_STUDIO_INIT_LIVEUPDATE,
	//			FMOD_INIT_PROFILE_ENABLE,
	//			nullptr));

	ERRCHECK(_studioSystem->getLowLevelSystem(&_system));

	FMOD::Studio::Bank* masterBank = nullptr;
	ERRCHECK(_studioSystem->loadBankFile("Assets/Audio/Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));


	FMOD::Studio::Bank* stringsBank = nullptr;
	ERRCHECK(_studioSystem->loadBankFile("Assets/Audio/Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank));

	FMOD::Studio::Bank* ambienceBank = nullptr;
	ERRCHECK(_studioSystem->loadBankFile("Assets/Audio/Character.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &ambienceBank));
	ambienceBank->loadSampleData();
	
	FMOD::Studio::EventDescription* desc[200];
	int count;

	ambienceBank->getEventList(desc, 200, &count);

	char path[1024];
	int retieved;

	for (int i = 0; i < count; i++)
	{
		ERRCHECK(desc[i]->getPath(path, 1024, &retieved));
		LOG("Path: %s", path);
	}
	

	FMOD::Studio::EventDescription* eventDescription = nullptr;
	ERRCHECK(_studioSystem->getEvent("event:/Character/Footsteps/Footsteps", &eventDescription));

	// Find the parameter once and then set by index
	// Or we can just find by name every time but by index is more efficient if we are setting lots of parameters
	FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
	ERRCHECK(eventDescription->getParameter("Surface", &paramDesc));
	int surfaceIndex = paramDesc.index;

	FMOD::Studio::EventInstance* eventInstance = nullptr;
	ERRCHECK(eventDescription->createInstance(&eventInstance));

	// Make the event audible to start with
	float surfaceParameterValue = 0;
	ERRCHECK(eventInstance->setParameterValueByIndex(surfaceIndex, 1.0f));
	ERRCHECK(eventInstance->getParameterValueByIndex(surfaceIndex, &surfaceParameterValue));

	ERRCHECK(eventInstance->start());
}

AudioManager::~AudioManager()
{
	ERRCHECK(_studioSystem->unloadAll());
	ERRCHECK(_studioSystem->release());
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
	
	ERRCHECK(_studioSystem->update());
}

void AudioManager::LoadBank(const std::string& bankName,
							FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
	auto foundIt = _banks.find(bankName);
	if (foundIt != _banks.end())
		return;

	FMOD::Studio::Bank* bank;
	ERRCHECK(_studioSystem->loadBankFile(bankName.c_str(), flags, &bank));
	if (bank) 		
		_banks[bankName] = bank;
}

void AudioManager::PlaySound(const std::string& soundName, const Vector3& vPos, float volumedB)
{

}

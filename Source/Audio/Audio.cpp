#include <Audio/Audio.h>
#include <fmod_studio.hpp>
#include <Defines.h>
#include <fmod_errors.h>
#include <Core/Transform.h>
#include <Physics/Physics2D.h>

#define VERBOSE_LEVEL 4

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

FMOD_VECTOR Osm::VectorToFmod(const Vector3& vector)
{
	FMOD_VECTOR v = { vector.x, vector.y, vector.z };
	return v;
}

FMOD_VECTOR Osm::VectorToFmod(const Vector2& vector)
{
	FMOD_VECTOR v = { vector.x, 0.0f, vector.y };
	return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	AudioManagerComponent
//
////////////////////////////////////////////////////////////////////////////////////////////////////

AudioManager::AudioManager(CGame& owner) : Component(owner)
{	
	ERRCHECK(FMOD::Studio::System::create(&_studioSystem));
	ERRCHECK(_studioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));

#if VERBOSE_LEVEL > 0
	LOG("FMOD Studio System Initialized.");
#endif

	/*
	//ERRCHECK(	_studioSystem->initialize(1028,
	//			FMOD_STUDIO_INIT_LIVEUPDATE,
	//			FMOD_INIT_PROFILE_ENABLE,
	//			nullptr));

	//ERRCHECK(_studioSystem->getLowLevelSystem(&_system));

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
	*/
}

AudioManager::~AudioManager()
{
	ERRCHECK(_studioSystem->unloadAll());
	ERRCHECK(_studioSystem->release());
}

void AudioManager::Update(float dt)
{


	if(_listener)
	{
		//_studioSystem->setListenerAttributes(0, )
	}
	else
	{
		
	}

	for (auto s : _sources)
	{
		auto posData = s->GetPositionalData();		
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

void AudioManager::PlayEvent(const std::string& eventName)
{
	LoadEvent(eventName);
	auto found = _events.find(eventName);
	if (found != _events.end())
		found->second->start();	
}

void AudioManager::LoadEvent(const std::string& eventName)
{
	auto found = _events.find(eventName);
	if (found != _events.end())
		return;

	FMOD::Studio::EventDescription* pEventDescription = nullptr;
	ERRCHECK(_studioSystem->getEvent(eventName.c_str(), &pEventDescription));
	if (pEventDescription)
	{
		FMOD::Studio::EventInstance* pEventInstance = nullptr;
		ERRCHECK(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance)
		{
			_events[eventName] = pEventInstance;
		}
	}
}


void AudioManager::Add(AudioSource* source)
{
	_sources.push_back(source);
}

void AudioManager::Remove(AudioSource* source)
{
}

void AudioManager::Set(AudioListener* listener)
{
}

void AudioManager::Remove(AudioListener* listener)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	AudioManagerComponent
//
////////////////////////////////////////////////////////////////////////////////////////////////////
AudioManagerComponent::AudioManagerComponent(Entity & entity)
	: Component(entity)
{
	Init();
}


void AudioManagerComponent::Update(float dt)
{
	if (_body2D)
	{
		_attributes.position = VectorToFmod(_body2D->GetPosition());
		_attributes.forward = VectorToFmod(_body2D->GetForward());
		_attributes.velocity = VectorToFmod(_body2D->GetVelocity());
		_attributes.up = { 0.0f, 1.0f, 0.0f };
	}
	else if (_transform)
	{
		Vector3 prevPos(_attributes.position.x, _attributes.position.y, _attributes.position.z);
		Matrix44 world = _transform->GetWorld();
		_attributes.position = VectorToFmod(_transform->GetPosition());
		_attributes.forward = VectorToFmod(world.TransformDirectionVector(Vector3(0.0f, 0.0f, 1.0f)));
		_attributes.velocity = { 0.0f, 0.0f, 0.0f };
		_attributes.up = VectorToFmod(world.TransformDirectionVector(Vector3(0.0f, 1.0f, 0.0f)));
	}
}

void AudioManagerComponent::Init()
{
	_transform = GetOwner().GetComponent<Transform>();
	_body2D = GetOwner().GetComponent<PhysicsBody2D>();

	if (_transform && !_body2D)
	{
		_attributes.position = VectorToFmod(_transform->GetPosition());
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	AudioSource
//
////////////////////////////////////////////////////////////////////////////////////////////////////

AudioSource::AudioSource(Entity& entity) : AudioManagerComponent(entity)
{
	Game.Audio().Add(this);
}

AudioSource::~AudioSource()
{
	Game.Audio().Remove(this);
}

bool AudioSource::LoadEvent(const std::string& eventpath)
{
	return false;
}

bool AudioSource::Play()
{
	return false;
}

AudioListener::AudioListener(Entity& entity) : AudioManagerComponent(entity)
{
	Game.Audio().Set(this);
}

AudioListener::~AudioListener()
{
	Game.Audio().Remove(this);
}

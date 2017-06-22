#include <Audio/Audio.h>
#include <fmod_studio.hpp>
#include <Defines.h>
#include <fmod_errors.h>
#include <Core/Transform.h>
#include <Physics/Physics2D.h>

#define VERBOSE_LEVEL 4

using namespace Osm;
using namespace std;
using namespace FMOD::Studio;

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
	ERRCHECK(_studioSystem->initialize(
		1024,
		FMOD_STUDIO_INIT_LIVEUPDATE,
		FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED,
		nullptr));
	ERRCHECK(_studioSystem->getLowLevelSystem(&_system));

#if VERBOSE_LEVEL > 0
	LOG("FMOD Studio System Initialization Complete.");
#endif

	_system->set3DSettings(1.0f, 1.0f, 0.1f);

	/*
	//ERRCHECK(	_studioSystem->initialize(1028,
	//			FMOD_STUDIO_INIT_LIVEUPDATE,
	//			FMOD_INIT_PROFILE_ENABLE,
	//			nullptr));

	

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

	// Update listener
	if(_listener)
	{		
		//_studioSystem->setListenerAttributes();
	}
	else
	{
		//_studioSystem->set
	}

	// Update sources
	for (auto s : _sources)
	{
		s->Update(dt);	
		s->UpdatePositionalData();
	}

	// Update one-off sounds
	for (auto itr = _events.begin(); itr != _events.end(); )
	{
		EventInstance* instance = *itr;
		FMOD_STUDIO_PLAYBACK_STATE state;
		ERRCHECK(instance->getPlaybackState(&state));
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			instance->release();
			itr = _events.erase(itr);
		}
		else
		{
			++itr;
		}
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

void AudioManager::PlayEvent(const std::string& eventName, const Vector3& position)
{
	FMOD::Studio::EventDescription* description = nullptr;
	ERRCHECK(_studioSystem->getEvent(eventName.c_str(), &description));
	if (description)
	{
		FMOD::Studio::EventInstance* instance = nullptr;
		ERRCHECK(description->createInstance(&instance));
		if (instance)
		{
			_events.push_back(instance);
			instance->start();
			FMOD_3D_ATTRIBUTES attributes;
			attributes.position = VectorToFmod(position);
			attributes.forward = { 0.0f, 0.0f, 1.0f };
			attributes.up = { 0.0f, 1.0f, 0.0f };
			attributes.velocity = { 0.0f, 0.0f, 0.0f };
			instance->set3DAttributes(&attributes);
		}
		else
		{
			LOG("Unable to create istance of sound event: %s", eventName.c_str());
		}
	}
	else
	{
		LOG("Unable to locate sound event description: %s", eventName.c_str());
	}
}

void AudioManager::LoadEvent(const std::string& eventName)
{
	/*
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
	*/
}

EventDescription* AudioManager::LoadDescription(const std::string& eventName)
{
	auto found = _descriptions.find(eventName);
	if (found != _descriptions.end())
		return found->second;

	EventDescription* description = nullptr;
	ERRCHECK(_studioSystem->getEvent(eventName.c_str(), &description));
	_descriptions[eventName] = description;
	return description;
}


void AudioManager::Add(AudioSource* source)
{
	_sources.push_back(source);
}

void AudioManager::Remove(AudioSource* source)
{
	_sources.erase(remove(_sources.begin(), _sources.end(), source));
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

bool AudioSource::LoadEvent(const std::string& eventPath)
{
	EventDescription* description = Game.Audio().LoadDescription(eventPath);
	if (description)
	{
		ERRCHECK(description->createInstance(&_event));
		if (_event)		
			return true;
	}

	return false;
}

void AudioSource::Play() const
{		
	ERRCHECK(_event->start());
}

void AudioSource::SetParameter(const std::string& name, float value) const
{
	ERRCHECK(_event->setParameterValue(name.c_str(), value));
}

void AudioSource::UpdatePositionalData() const
{
	_event->set3DAttributes(&_attributes);
}

AudioListener::AudioListener(Entity& entity) : AudioManagerComponent(entity)
{
	Game.Audio().Set(this);
}

AudioListener::~AudioListener()
{
	Game.Audio().Remove(this);
}

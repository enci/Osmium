#pragma once

#include <Core/Game.h>
#include <Math/Vector3.h>
#include <Core/Entity.h>
#include <fmod_studio.hpp>

namespace Osm
{
class PhysicsBody2D;
class Transform;
class Vector2;
class AudioSource;
class AudioListener;

FMOD_VECTOR VectorToFmod(const Vector3& vector);

FMOD_VECTOR VectorToFmod(const Vector2& vector);

class AudioManager : Component<CGame>
{
public:
	AudioManager(CGame& owner);

	virtual ~AudioManager();

	void Update(float dt);

	void LoadBank(const std::string& bankName,
		FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);

	void PlayEvent(const std::string& eventName, const Vector3& position);

	void LoadEvent(const std::string& eventName);

	FMOD::Studio::EventDescription* LoadDescription(const std::string& eventName);

	//void LoadEvent(const std::string& eventName);

	/*
	void PlaySound(	const std::string& soundName,
					const Vector3& vPos = Vector3(),
					float volumedB = 0.0f);
	
	void Loadsound(	const std::string& soundName,
					bool b3d = true,
					bool bLooping = false,
					bool bStream = false);
	void UnLoadSound(const std::string& soundName);
	void Set3dListenerAndOrientation(	const Vector3& vPos = Vector3(),
										float fVolumedB = 0.0f);
	

	void PlayEvent(const std::string& eventName);
	void StopChannel(int channelId);
	void StopEvent(	const std::string& eventName,
					bool immediate = false);
	void GetEventParameter(	const std::string& eventName,
							const std::string& eventParameter,
							float* parameter);

	void SetEventParameter(	const std::string& eventName,
							const std::string& parameterName,
							float value);
	void StopAllChannels();
	void SetChannel3dPosition(int channelId, const Vector3& position);
	void SetChannelvolume(int channelId, float volumedB);
	bool IsPlaying(int channelId) const;
	bool IsEventPlaying(const std::string& eventName) const;
	float dbToVolume(float db);
	float VolumeTodb(float volume);
	FMOD_VECTOR VectorToFmod(const Vector3& vPosition);
	*/

	void Add(AudioSource* source);

	void Remove(AudioSource* source);

	void Set(AudioListener* listener);

	void Remove(AudioListener* listener);

private:
	
	// std::map<std::string, FMOD::Sound*>						_sounds;
	// std::map<int, FMOD::Channel*>							_channels;

	struct DescriptionEntry
	{
		FMOD::Studio::EventDescription* Description;
		float Cooldown;
	};

	FMOD::System*													_system = nullptr;
	FMOD::Studio::System*											_studioSystem = nullptr;	

	//std::unordered_map<std::string, FMOD::Studio::EventInstance*>	_events;	

	std::unordered_map<std::string, DescriptionEntry>	_descriptions;

	std::unordered_map<std::string, FMOD::Studio::Bank*>				_banks;
	std::vector<FMOD::Studio::EventInstance*>							_events;

	std::vector<AudioSource*>											_sources;
	AudioListener*														_listener	= nullptr;
};


///
/// AudioManagerComponent
/// A small utility to calculate positional data for audio
///
class AudioManagerComponent : public Component<Entity>
{
public:

	AudioManagerComponent(Entity& entity);

	virtual ~AudioManagerComponent() {}
	
	void Update(float dt);

	const FMOD_3D_ATTRIBUTES& GetPositionalData() const { return _attributes; }

protected:

	void Init();

	Transform*			_transform		= nullptr;
	PhysicsBody2D*		_body2D			= nullptr;
	FMOD_3D_ATTRIBUTES	_attributes;
};

///
/// AudioSource
/// TODO: Document this further
///
class AudioSource : public AudioManagerComponent
{
public:
	/// Creates a new audio source and adds it to the audio manager
	/// When deletes it will automatically get removed.
	AudioSource(Entity& entity);

	virtual ~AudioSource();

	bool LoadEvent(const std::string& eventPath);

	void Play() const;

	void SetVolume(float vol) const;

	void SetPitch(float pitch) const;

	void SetParameter(const std::string& name, float value) const;

	void UpdatePositionalData() const;

#ifdef INSPECTOR
	virtual void Inspect() override {}
#endif

protected:
	FMOD::Studio::EventInstance*	_event = nullptr;
};

///
/// AudioListener
/// TODO: Document this further
///
class AudioListener : public AudioManagerComponent
{
public:

	AudioListener(Entity& entity);

	virtual ~AudioListener();

#ifdef INSPECTOR
	virtual void Inspect() override {}
#endif

protected:
};

}

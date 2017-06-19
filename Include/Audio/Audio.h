#pragma once

#include <Core/Game.h>
#include <Math/Vector3.h>
#include <Core/Entity.h>
#include <Core/World.h>
#include <fmod_studio.hpp>
#include <map>
#include "Graphics/Render.h"

namespace Osm
{

struct Vector3;
class AudioSource;
class AudioListener;

//	FMOD_VECTOR 
class AudioManager : Component<CGame>
{
public:
	AudioManager(CGame& owner);

	virtual ~AudioManager();

	void Update(float dt);

	void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);

	void LoadEvent(const std::string& eventName);

	void Loadsound(	const std::string& soundName,
					bool b3d = true,
					bool bLooping = false,
					bool bStream = false);

	void UnLoadSound(const std::string& soundName);

	void Set3dListenerAndOrientation(	const Vector3& vPos = Vector3(),
										float fVolumedB = 0.0f);

	void PlaySound(	const std::string& soundName,
					const Vector3& vPos = Vector3(),
					float volumedB = 0.0f);

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

	void Add(AudioSource* source);

	void Remove(AudioSource* source);

	void Add(AudioListener* listener);

	void Remove(AudioListener* listener);



private:
	FMOD::System*										_system			= nullptr;
	FMOD::Studio::System*								_studioSystem	= nullptr;
	std::map<std::string, FMOD::Sound*>					_sounds;
	std::map<int, FMOD::Channel*>						_channels;
	std::map<std::string, FMOD::Studio::EventInstance*> _events;
	std::map<std::string, FMOD::Studio::Bank*>			_banks;
};


///
/// AudioManagerComponent
/// A small utility to make sure object add/remove themselves from
/// the manager.
///
template<class T>
class AudioManagerComponent : public Component<Entity>
{
public:

	AudioManagerComponent(Entity& entity) : Component<Entity>(entity)
	{
		auto rmng = _owner.GetWorld().GetComponent<AudioManager>();
		ASSERT(rmng);
		rmng->Add(static_cast<T*>(this));
	}

	virtual ~AudioManagerComponent()
	{
		auto rmng = _owner.GetWorld().GetComponent<AudioManager>();
		ASSERT(rmng);
		rmng->Remove(static_cast<T*>(this));
	}
};

///
/// AudioSource
/// TODO: Document this further
///
class AudioSource : public AudioManagerComponent<AudioSource>
{
public:
	/// Creates a new renderable and adds it to the render manager. 
	/// When deletes it will automatically get removed.
	AudioSource(Entity& entity);

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

protected:
};

///
/// AudioListener
/// TODO: Document this further
///
class AudioListener : public AudioManagerComponent<AudioListener>
{
public:

	AudioListener(Entity& entity);

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

protected:
};

}

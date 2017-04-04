#pragma once

#include <Core/Game.h>
#include <Math/Vector3.h>
#include <fmod_studio.hpp>
#include <map>

namespace Osm
{
	struct Vector3;

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

private:
	FMOD::System*										_system			= nullptr;
	FMOD::Studio::System*								_studioSystem	= nullptr;
	std::map<std::string, FMOD::Sound*>					_sounds;
	std::map<int, FMOD::Channel*>						_channels;
	std::map<std::string, FMOD::Studio::EventInstance*> _events;
	std::map<std::string, FMOD::Studio::Bank*>			_banks;
};

}
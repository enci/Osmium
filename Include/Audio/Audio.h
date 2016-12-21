#pragma once
#include <fmod.h>
#include <Core/Engine.h>

namespace Osm
{

class AudioManager : Component<CEngine>
{
public:
	AudioManager(CEngine& owner);
};

}
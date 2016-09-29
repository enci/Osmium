#pragma once
#include <Core/Component.h>
#include <Core/Engine.h>

namespace Osm
{

class Inspector : public Component<CEngine>
{
public:
	Inspector(CEngine & engine);
};

}
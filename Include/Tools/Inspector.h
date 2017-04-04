#pragma once
#include <Core/Component.h>
#include <Core/Game.h>

namespace Osm
{

class Inspector : public Component<CGame>
{
public:
	Inspector(CGame & engine);
};

}
#pragma once

#include <Graphics/Color.h>

namespace Osm
{
	template<class Archive>
	void serialize(Archive& archive, Color& c)
	{
		archive(c.integervalue);
	}
}

template<class Archive>
void serialize(Archive& archive, Osm::Color& c)
{
	archive(c.integervalue);
}


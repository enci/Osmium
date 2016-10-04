#pragma once
#include <Core/Engine.h>

namespace Osm
{

class Profiler : public  Component<CEngine>
{
public:
	Profiler(CEngine& engine);

	void StartFrame();

	uint StartSection(const std::string& name);

	void EndSection(uint sectionId);

#ifdef INSPECTOR
	virtual void Inspect() override;
#endif

private:

	struct Section
	{
		std::string Name;
		double StartTime;
		double EndTime;
	};

	std::vector<Section> _sections;
	double _frameStart;
};

}

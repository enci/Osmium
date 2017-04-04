#pragma once
#include <Core/Game.h>
#include <tchar.h>

namespace Osm
{

class Profiler : public  Component<CGame>
{
public:
	Profiler(CGame& engine);

	void StartFrame();

	void EndFrame();

	uint StartSection(const std::string& name);

	void EndSection(uint sectionId);

	double GetTimePerFrame() const	{ return _timePerFrame;		}

	double GetFPS() const;

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

	std::vector<Section>	_sections;
	double					_frameStart;
	double					_frameEnd;
	double					_framePerSecond;
	double					_timePerFrame;
	double					_timeSinceRefresh;
};

}

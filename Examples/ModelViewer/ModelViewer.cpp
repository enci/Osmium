#define INSPECTOR

//#include <vld.h>
#include <glad/glad.h>
#include <iostream>
#include <Core/Resources.h>
#include <Core/Game.h>

using namespace Osm;

int main(void)
{
	GameSettings settings;
	settings.LoadFromFile("Settings.json");
	settings.WindowName = "Shader Tool";

	// FreeConsole();	

	Game.Initialize(settings);
	//Game.SwapWorld(new ToolWorld());
	//Game.Run();
	Game.Shutdown();

	exit(EXIT_SUCCESS);
}

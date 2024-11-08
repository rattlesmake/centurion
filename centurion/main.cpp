// to hide the cmd:
// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <engine.h>

//#ifndef __MSXML_LIBRARY_DEFINED__
//#define __MSXML_LIBRARY_DEFINED__
//#endif
//#include <windows.h>
//#pragma comment(lib, "winmm.lib")

// main function

int main(int numArgs, char* args[])
{
	//timeBeginPeriod(1);

	Engine& engine = Engine::GetInstance();

	if (engine.Initialize(args[0]))
	{
		return engine.Launch();
	}
	else
	{
		return 1;
	}
}

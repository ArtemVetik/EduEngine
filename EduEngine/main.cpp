#include "EduEngine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	EduEngine::Common::AllocWinConsole();
#endif

	EduEngine::EduEngine engine(hInstance);
	engine.Run();

	return 0;
}
#include "EduEngine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	EduEngine::Common::AllocWinConsole();
#endif

	std::wstring folderPath = EduEngine::Common::OpenFolderDialog(true, L"Select Assets folder");
	if (folderPath.empty())
		return 0;

	std::wstring dllPath = EduEngine::Common::OpenFolderDialog(false, L"Select scripts DLL file");
	if (dllPath.empty())
		return 0;

	EduEngine::EduEngine engine(hInstance, folderPath, dllPath);
	engine.Run();

	return 0;
}
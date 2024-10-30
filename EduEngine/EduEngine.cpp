#include <Windows.h>
#include <crtdbg.h>
#include "../Graphics/Window.h"
#include "../Graphics/RenderEngine.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	FILE* fp;

	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
#endif

	EduEngine::Window window(hInstance);
	window.Initialize();

	EduEngine::InputManager::GetInstance().Initialize(hInstance, window.GetMainWindow());

	EduEngine::RenderEngine game;
	game.StartUp();

	EduEngine::Timer timer = EduEngine::Timer(window.GetMainWindow(), L"EduEngine");

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.UpdateTimer();
			if (!window.IsPaused())
			{
				timer.UpdateTitleBarStats();
				game.Update();
				game.Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return 0;
}
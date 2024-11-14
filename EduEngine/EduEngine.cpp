#include <Windows.h>
#include <crtdbg.h>
#include "../InputSystem/Timer.h"
#include "../InputSystem/InputManager.h"
#include "../RenderEngine/Window.h"
#include "../RenderEngine/IRenderEngine.h"
#include "../RenderEngine/Camera.h"
#include "../RenderEngine/GeometryGenerator.h"
#include "../Physics/PhysicsFactory.h"
#include "../CoreInterop/CoreSystems.h"
#include "../GameplayInterop/GameplayInterop.h"

using namespace EduEngine;

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

	GeometryGenerator geoGen;

	Window window(hInstance);
	window.Initialize();

	InputManager::GetInstance().Initialize(hInstance, window.GetMainWindow());

	std::shared_ptr<IRenderEngine> renderEngine = IRenderEngine::Create(window);

	PhysicsFactory physicsFactory;
	std::shared_ptr<IPhysicsWorld> physicsWorld = physicsFactory.Create();
	EduEngine::Timer timer = EduEngine::Timer(window.GetMainWindow(), L"EduEngine");

	EduEngine::CoreSystems s(renderEngine.get(), physicsWorld.get(), &timer);

	GameplayInterop::Initialize();

	const float fixedTimeStep = 1.0f / 60.0f;
	float physixsAccumulator = 0.0f;

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

				InputManager::GetInstance().Update();

				physixsAccumulator += timer.GetDeltaTime();

				if (physixsAccumulator >= fixedTimeStep)
				{
					physicsWorld->Update();
					physixsAccumulator = 0.0f;
				}

				GameplayInterop::Update();

				renderEngine->Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return 0;
}
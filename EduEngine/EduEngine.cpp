#include <Windows.h>
#include <crtdbg.h>
#include "../InputSystem/Timer.h"
#include "../InputSystem/InputManager.h"
#include "../RenderEngine/RuntimeWindow.h"
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

	EditorWindow editorWindow(hInstance, 1280, 720);
	editorWindow.Initialize();

	RuntimeWindow runtimeWindow(hInstance, 800, 400, editorWindow.GetMainWindow());
	runtimeWindow.Initialize();

	InputManager::GetInstance().Initialize(hInstance, editorWindow.GetMainWindow());

	std::shared_ptr<IRenderEngine> renderEngine = IRenderEngine::Create(runtimeWindow);
	std::shared_ptr<IEditorRenderEngine> editorRenderEngine = IEditorRenderEngine::Create(editorWindow);

	PhysicsFactory physicsFactory;
	std::shared_ptr<IPhysicsWorld> physicsWorld = physicsFactory.Create();
	Timer timer = Timer(runtimeWindow.GetMainWindow(), L"EduEngine");

	CoreSystems coreSystems(renderEngine.get(), editorRenderEngine.get(), physicsWorld.get(), &timer);

	GameplayInterop::Initialize();
	EditorInterop::Initialize();

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
			if (!editorWindow.IsPaused())
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
				EditorInterop::Update();

				renderEngine->Draw();
				editorRenderEngine->Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	EditorInterop::Destroy();

	return 0;
}
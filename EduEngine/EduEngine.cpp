#include <Windows.h>
#include <crtdbg.h>
#include <thread>
#include <future>
#include <sstream>
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

void UpdateWindowTitle(HWND window, int rFps, float rMspf, int eFps, float eMspf)
{
	std::wstringstream out;
	out.precision(6);

	out << "Runtime (" << " fps: " << rFps << " frame time: " << rMspf << " ms)" <<
		   "\tEditor (" << "fps: " << eFps << " frame time: " << eMspf << " ms)" << "\0";

	SetWindowText(window, out.str().c_str());
}

template<typename T>
bool future_is_ready(std::future<T>& t) {
	return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

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

	InputManager::GetEditorInstance().Initialize(hInstance, editorWindow.GetMainWindow());
	InputManager::GetInstance().Initialize(hInstance, runtimeWindow.GetMainWindow());

	std::shared_ptr<IRenderEngine> renderEngine = IRenderEngine::Create(runtimeWindow);
	std::shared_ptr<IEditorRenderEngine> editorRenderEngine = IEditorRenderEngine::Create(editorWindow);

	PhysicsFactory physicsFactory;
	std::shared_ptr<IPhysicsWorld> physicsWorld = physicsFactory.Create();

	Timer runtimeTimer = Timer(runtimeWindow.GetMainWindow(), L"EduEngine");
	Timer editorTimer = Timer(editorWindow.GetMainWindow(), L"EduEngine");

	CoreSystems coreSystems(renderEngine.get(), editorRenderEngine.get(), physicsWorld.get(), &runtimeTimer);

	GameplayInterop::Initialize();
	EditorInterop::Initialize();

	const float fixedTimeStep = 1.0f / 60.0f;
	float physixsAccumulator = 0.0f;

	MSG msg = { 0 };

	std::future<void> runtimeThread;
	std::future<void> editorThread;

	int runtimeFps = 0, editorFps = 0;
	float runtimeMspf = 0.0f, editorMspf = 0.0f;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!runtimeThread.valid() || future_is_ready(runtimeThread))
			{
				runtimeTimer.UpdateTimer();
				if (!runtimeWindow.IsPaused())
				{
					if (runtimeTimer.UpdateTitleBarStats(runtimeFps, runtimeMspf))
						UpdateWindowTitle(editorWindow.GetHostWindow(), runtimeFps, runtimeMspf, editorFps, editorMspf);

					InputManager::GetInstance().Update();

					physixsAccumulator += runtimeTimer.GetDeltaTime();

					if (physixsAccumulator >= fixedTimeStep)
					{
						physicsWorld->Update();
						physixsAccumulator = 0.0f;
					}

					GameplayInterop::Update();
					runtimeThread = std::async(std::launch::async, &IRenderEngine::Draw, renderEngine.get());
				}
				else
				{
					Sleep(100);
				}
			}
			if (!editorThread.valid() || future_is_ready(editorThread))
			{
				editorTimer.UpdateTimer();
				if (!editorWindow.IsPaused())
				{
					if (editorTimer.UpdateTitleBarStats(editorFps, editorMspf))
						UpdateWindowTitle(editorWindow.GetHostWindow(), runtimeFps, runtimeMspf, editorFps, editorMspf);

					InputManager::GetEditorInstance().Update();
					
					EditorInterop::Update();
					editorThread = std::async(std::launch::async, &IEditorRenderEngine::Draw, editorRenderEngine.get());
				}
				else
				{
					Sleep(100);
				}
			}
		}
	}

	editorThread.get();
	runtimeThread.get();

	EditorInterop::Destroy();

	return 0;
}
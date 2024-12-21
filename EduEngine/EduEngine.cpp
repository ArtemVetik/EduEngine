#include <Windows.h>
#include <thread>
#include "Common.h"
#include "RuntimeRender.h"
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
	AllocWinConsole();
#endif

	std::wstring folderPath = OpenFolderDialog();
	if (folderPath.empty())
		return 0;

	std::wstring dllPath = OpenFolderDialog(false);
	if (dllPath.empty())
		return 0;

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

	CoreSystems coreSystems(renderEngine.get(), editorRenderEngine.get(), physicsWorld.get(), &runtimeTimer, &editorTimer);

	EditorInterop::Initialize(folderPath, dllPath);

	RuntimeRender runtimeRender(renderEngine.get());

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
					InputManager::GetInstance().Update();

					if (runtimeTimer.UpdateTitleBarStats(runtimeFps, runtimeMspf))
						UpdateWindowTitle(editorWindow.GetHostWindow(), runtimeFps, runtimeMspf, editorFps, editorMspf);

					if (EditorInterop::GetEngineState() == EngineState::Runtime)
					{
						physixsAccumulator += runtimeTimer.GetDeltaTime();

						if (physixsAccumulator >= fixedTimeStep)
						{
							physicsWorld->Update();
							physixsAccumulator = 0.0f;
						}

						GameplayInterop::Update();

						if (EditorInterop::InspectScene())
							runtimeThread = std::async(std::launch::async, &RuntimeRender::RenderEditor, runtimeRender);
						else
							runtimeThread = std::async(std::launch::async, &RuntimeRender::RenderRuntime, runtimeRender);
					}
					else if (EditorInterop::GetEngineState() == EngineState::Editor)
					{
						GameplayInterop::Update();
						runtimeThread = std::async(std::launch::async, &RuntimeRender::RenderEditor, runtimeRender);
					}
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
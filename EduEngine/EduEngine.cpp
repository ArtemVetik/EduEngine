#include <Windows.h>
#include <thread>
#include "Common.h"
#include "RuntimeRender.h"
#include "../Modules/InputSystem/InputManager.h"
#include "../Modules/RenderEngine/Timer.h"
#include "../Modules/RenderEngine/RuntimeWindow.h"
#include "../Modules/RenderEngine/IRenderEngine.h"
#include "../Modules/RenderEngine/Camera.h"
#include "../Modules/RenderEngine/GeometryGenerator.h"
#include "../Modules/Physics/PhysicsFactory.h"
#include "../Modules/CoreInterop/CoreSystems.h"
#include "../Modules/GameplayInterop/GameplayInterop.h"

#if 0
#define EDU_NO_EDITOR
#endif

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

	HWND editorWnd = 0;
#ifndef EDU_NO_EDITOR
	EditorWindow editorWindow(hInstance, 1280, 720);
	editorWindow.Initialize();
	editorWnd = editorWindow.GetMainWindow();
#endif

	RuntimeWindow runtimeWindow(hInstance, 1280, 720, editorWnd);
	runtimeWindow.Initialize();

#ifndef EDU_NO_EDITOR
	InputManager::GetEditorInstance().Initialize(hInstance, editorWindow.GetMainWindow());
#endif
	InputManager::GetInstance().Initialize(hInstance, runtimeWindow.GetMainWindow());

	Timer runtimeTimer = Timer(runtimeWindow.GetMainWindow(), L"EduEngine");
#ifndef EDU_NO_EDITOR
	Timer editorTimer = Timer(editorWindow.GetMainWindow(), L"EduEngine");
#endif

	std::shared_ptr<IRenderEngine> renderEngine = IRenderEngine::Create(runtimeWindow, runtimeTimer);
#ifndef EDU_NO_EDITOR
	std::shared_ptr<IEditorRenderEngine> editorRenderEngine = IEditorRenderEngine::Create(editorWindow);
#endif

	PhysicsFactory physicsFactory;
	std::shared_ptr<IPhysicsWorld> physicsWorld = physicsFactory.Create();

#ifndef EDU_NO_EDITOR
	CoreSystems coreSystems(renderEngine.get(), editorRenderEngine.get(), physicsWorld.get(), &runtimeTimer, &editorTimer);
#else
	CoreSystems coreSystems(renderEngine.get(), nullptr, physicsWorld.get(), &runtimeTimer, nullptr);
#endif

#ifndef EDU_NO_EDITOR
	EditorInterop::Initialize(folderPath, dllPath, true);
#else
	EditorInterop::Initialize(folderPath, dllPath, false);
#endif

	RuntimeRender runtimeRender(renderEngine.get());

	const float fixedTimeStep = 1.0f / 120.0f;
	float physixsAccumulator = 0.0f;

	MSG msg = { 0 };

	std::future<void> runtimeThread;
#ifndef EDU_NO_EDITOR
	std::future<void> editorThread;
#endif

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
#ifndef EDU_NO_EDITOR
			if (!runtimeThread.valid() || future_is_ready(runtimeThread))
#endif
			{
				runtimeTimer.UpdateTimer();
				if (!runtimeWindow.IsPaused())
				{
					InputManager::GetInstance().Update();

					if (runtimeTimer.UpdateTitleBarStats(runtimeFps, runtimeMspf))
						UpdateWindowTitle(runtimeWindow.GetMainWindow(), runtimeFps, runtimeMspf, nullptr, nullptr);

#ifndef EDU_NO_EDITOR
					if (EditorInterop::GetEngineState() == EngineState::Runtime)
#endif
					{
						physixsAccumulator += runtimeTimer.GetDeltaTime();

						if (physixsAccumulator >= fixedTimeStep)
						{
							physicsWorld->Update();
							physixsAccumulator = 0.0f;
						}

						GameplayInterop::Update();

#ifndef EDU_NO_EDITOR
						if (EditorInterop::InspectScene())
						{
							runtimeThread = std::async(std::launch::async, &RuntimeRender::RenderEditor, runtimeRender);
						}
						else
#endif
						{
#ifndef EDU_NO_EDITOR
							runtimeThread = std::async(std::launch::async, &RuntimeRender::RenderRuntime, runtimeRender);
#else
							runtimeRender.RenderRuntime();
#endif
						}
					}
#ifndef EDU_NO_EDITOR
					else if (EditorInterop::GetEngineState() == EngineState::Editor)
					{
						GameplayInterop::Update();
						runtimeThread = std::async(std::launch::async, &RuntimeRender::RenderEditor, runtimeRender);
					}
#endif
				}
				else
				{
					Sleep(100);
				}
			}
#ifndef EDU_NO_EDITOR
			if (!editorThread.valid() || future_is_ready(editorThread))
			{
				editorTimer.UpdateTimer();
				if (!editorWindow.IsPaused())
				{
					if (editorTimer.UpdateTitleBarStats(editorFps, editorMspf))
						UpdateWindowTitle(editorWindow.GetHostWindow(), runtimeFps, runtimeMspf, &editorFps, &editorMspf);

					InputManager::GetEditorInstance().Update();

					EditorInterop::Update();
					editorThread = std::async(std::launch::async, &IEditorRenderEngine::Draw, editorRenderEngine.get());
				}
				else
				{
					Sleep(100);
				}
			}
#endif
		}
	}

#ifndef EDU_NO_EDITOR
	editorThread.get();
	runtimeThread.get();

	editorRenderEngine.reset();
#endif

	EditorInterop::Destroy();
	GameplayInterop::Destroy();

	renderEngine.reset();
	physicsWorld.reset();

	return 0;
}
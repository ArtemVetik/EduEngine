#include "EduEngine.h"

namespace EduEngine
{
	EduEngine::EduEngine(HINSTANCE hInstance, std::wstring folderPath, std::wstring dllPath)
	{
		HWND editorWnd = 0;
#ifndef EDU_NO_EDITOR
		m_EditorWindow = std::make_unique<EditorWindow>(hInstance, 1280, 720);
		m_EditorWindow->Initialize();
		editorWnd = m_EditorWindow->GetMainWindow();
#endif

		m_RuntimeWindow = std::make_unique<RuntimeWindow>(hInstance, 1280, 720, editorWnd);
		m_RuntimeWindow->Initialize();

#ifndef EDU_NO_EDITOR
		InputManager::GetEditorInstance().Initialize(hInstance, m_EditorWindow->GetMainWindow());
#endif
		InputManager::GetInstance().Initialize(hInstance, m_RuntimeWindow->GetMainWindow());

		m_RuntimeTimer = std::make_unique<Timer>(m_RuntimeWindow->GetMainWindow(), L"EduEngine");
#ifndef EDU_NO_EDITOR
		m_EditorTimer = std::make_unique<Timer>(m_EditorWindow->GetMainWindow(), L"EduEngine");
#endif
		m_RenderEngine = IRenderEngine::Create(*m_RuntimeWindow, *m_RuntimeTimer);
#ifndef EDU_NO_EDITOR
		m_EditorRenderEngine = IEditorRenderEngine::Create(*m_EditorWindow);
#endif

		PhysicsFactory physicsFactory;
		m_PhysicsWorld = physicsFactory.Create();

#ifndef EDU_NO_EDITOR
		m_CoreSystems = std::make_unique<CoreSystems>(m_RenderEngine.get(), m_EditorRenderEngine.get(), m_PhysicsWorld.get(), m_RuntimeTimer.get(), m_EditorTimer.get());
#else
		m_CoreSystems = std::make_unique<CoreSystems>(m_RenderEngine.get(), nullptr, m_PhysicsWorld.get(), m_RuntimeTimer.get(), nullptr);
#endif

#ifndef EDU_NO_EDITOR
		EditorInterop::Initialize(folderPath, dllPath, true);
#else
		EditorInterop::Initialize(folderPath, dllPath, false);
#endif
	}

	EduEngine::~EduEngine()
	{
#ifndef EDU_NO_EDITOR
		m_EditorThread.get();

		m_EditorRenderEngine.reset();
#endif

		EditorInterop::Destroy();
		GameplayInterop::Destroy();

		m_RenderEngine.reset();
		m_PhysicsWorld.reset();
	}

	void EduEngine::Run()
	{
		static MSG msg = { 0 };

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				RenderRuntime();

#ifndef EDU_NO_EDITOR
				if (!m_EditorThread.valid() || Common::future_is_ready(m_EditorThread))
				{
					RenderEditor();
				}
#endif
			}
		}
	}

	void EduEngine::RenderRuntime()
	{
		m_RuntimeTimer->UpdateTimer();
		if (!m_RuntimeWindow->IsPaused())
		{
			InputManager::GetInstance().Update();

			if (m_RuntimeTimer->UpdateTitleBarStats(m_RuntimeFps, m_RuntimeMspf))
				Common::UpdateWindowTitle(m_RuntimeWindow->GetMainWindow(), m_RuntimeFps, m_RuntimeMspf, nullptr, nullptr);

#ifndef EDU_NO_EDITOR
			if (EditorInterop::GetEngineState() == EngineState::Runtime)
#endif
			{
				m_PhysixsAccumulator += m_RuntimeTimer->GetDeltaTime();

				if (m_PhysixsAccumulator >= m_FixedTimeStep)
				{
					GameplayInterop::PhysicsUpdate();
					m_PhysicsWorld->Update();
					m_PhysixsAccumulator = 0.0f;
				}

				GameplayInterop::Update();

#ifndef EDU_NO_EDITOR
				if (EditorInterop::InspectScene())
				{
					m_RenderEngine->BeginDraw();
					EditorInterop::RenderScene();
					m_RenderEngine->EndDraw();
				}
				else
#endif
				{
					m_RenderEngine->BeginDraw();
					GameplayInterop::Render();
					m_RenderEngine->EndDraw();
				}
			}
#ifndef EDU_NO_EDITOR
			else if (EditorInterop::GetEngineState() == EngineState::Editor)
			{
				GameplayInterop::Update();
				m_RenderEngine->BeginDraw();
				EditorInterop::RenderScene();
				m_RenderEngine->EndDraw();
			}
#endif
		}
		else
		{
			Sleep(100);
		}
	}

	void EduEngine::RenderEditor()
	{
#ifndef EDU_NO_EDITOR
		m_EditorTimer->UpdateTimer();
		if (!m_EditorWindow->IsPaused())
		{
			if (m_EditorTimer->UpdateTitleBarStats(m_EditorFps, m_EditorMspf))
				Common::UpdateWindowTitle(m_EditorWindow->GetHostWindow(), m_RuntimeFps, m_RuntimeMspf, &m_EditorFps, &m_EditorMspf);

			InputManager::GetEditorInstance().Update();

			EditorInterop::Update();
			m_EditorThread = std::async(std::launch::async, &IEditorRenderEngine::Draw, m_EditorRenderEngine.get());
		}
		else
		{
			Sleep(100);
		}
#endif
	}
}
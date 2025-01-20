#pragma once
#include <Windows.h>
#include <thread>
#include "Common.h"
#include "RuntimeRender.h"
#include "../Modules/InputSystem/InputManager.h"
#include "../Modules/RenderEngine/RuntimeWindow.h"
#include "../Modules/Physics/PhysicsFactory.h"
#include "../Modules/CoreInterop/CoreSystems.h"

#if 0
#define EDU_NO_EDITOR
#endif

namespace EduEngine
{
	class EduEngine
	{
	public:
		EduEngine(HINSTANCE hInstance);
		~EduEngine();

		void Run();

	private:
		void RenderRuntime();
		void RenderEditor();

	private:
#ifndef EDU_NO_EDITOR
		std::future<void> m_RuntimeThread;
		std::future<void> m_EditorThread;

		std::unique_ptr<EditorWindow> m_EditorWindow;
		std::shared_ptr<IEditorRenderEngine> m_EditorRenderEngine;
		std::unique_ptr<Timer> m_EditorTimer;
#endif
		std::unique_ptr<RuntimeWindow> m_RuntimeWindow;
		std::shared_ptr<IRenderEngine> m_RenderEngine;
		std::shared_ptr<IPhysicsWorld> m_PhysicsWorld;
		std::unique_ptr<RuntimeRender> m_RuntimeRender;
		std::unique_ptr<Timer> m_RuntimeTimer;
		std::unique_ptr<CoreSystems> m_CoreSystems;

		const float m_FixedTimeStep = 1.0f / 120.0f;
		float m_PhysixsAccumulator = 0.0f;
		int m_RuntimeFps = 0;
		int m_EditorFps = 0;
		float m_RuntimeMspf = 0.0f;
		float m_EditorMspf = 0.0f;
	};
}
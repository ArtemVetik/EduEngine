#pragma once

#include "../RenderEngine/IRenderEngine.h"
#include "../Physics/IPhysicsWorld.h"
#include "../InputSystem/Timer.h"

#ifdef COREINTERLOP_EXPORTS
#define COREINTERLOP_API __declspec(dllexport)
#else
#define COREINTERLOP_API __declspec(dllimport)
#endif

namespace EduEngine
{
	class COREINTERLOP_API CoreSystems
	{
	public:
		CoreSystems(IRenderEngine*		renderEngine,
					IEditorRenderEngine* editorRenderEngine,
					IPhysicsWorld*		physicsWorld,
					Timer*				runtimeTimer,
					Timer*				editorTimer);

		static CoreSystems* GetInstance();

		IRenderEngine* GetRenderEngine() const { return m_RenderEngine; }
		IEditorRenderEngine* GetEditorRenderEngine() const { return m_EditorRenderEngine; }
		IPhysicsWorld* GetPhysicsWorld() const { return m_PhysicsWorld; }
		Timer* GetRuntimeTimer() const { return m_RuntimeTimer; }
		Timer* GetEditorTimer() const { return m_EditorTimer; }

	private:
		static CoreSystems* m_Instance;

		IRenderEngine* m_RenderEngine;
		IEditorRenderEngine* m_EditorRenderEngine;
		IPhysicsWorld* m_PhysicsWorld;
		Timer* m_RuntimeTimer;
		Timer* m_EditorTimer;
	};
}
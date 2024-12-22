#include "CoreSystems.h"

namespace EduEngine
{
	CoreSystems* CoreSystems::m_Instance = nullptr;

	CoreSystems::CoreSystems(IRenderEngine*		  renderEngine,
							 IEditorRenderEngine* editorRenderEngine,
							 IPhysicsWorld*		  physicsWorld,
							 Timer*				  runtimeTimer,
							 Timer*				  editorTimer)
	{
		assert(m_Instance == nullptr);

		m_RenderEngine = renderEngine;
		m_EditorRenderEngine = editorRenderEngine;
		m_PhysicsWorld = physicsWorld;
		m_RuntimeTimer = runtimeTimer;
		m_EditorTimer = editorTimer;
		m_Instance = this;
	}

	CoreSystems* CoreSystems::GetInstance()
	{
		return m_Instance;
	}
}
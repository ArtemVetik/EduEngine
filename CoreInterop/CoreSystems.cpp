#include "CoreSystems.h"

namespace EduEngine
{
	CoreSystems* CoreSystems::m_Instance = nullptr;

	CoreSystems::CoreSystems(IRenderEngine* renderEngine, IPhysicsWorld* physicsWorld, Timer* timer)
	{
		assert(m_Instance == nullptr);

		m_RenderEngine = renderEngine;
		m_PhysicsWorld = physicsWorld;
		m_Timer = timer;
		m_Instance = this;
	}

	CoreSystems* CoreSystems::GetInstance()
	{
		return m_Instance;
	}
}
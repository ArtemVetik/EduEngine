#pragma once

#include "../RenderEngine/IRenderEngine.h"
#include "../Physics/IPhysicsWorld.h"

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
		CoreSystems(IRenderEngine* renderEngine, IPhysicsWorld* physicsWorld);

		static CoreSystems* GetInstance();

		IRenderEngine* GetRenderEngine() const { return m_RenderEngine; }
		IPhysicsWorld* GetPhysicsWorld() const { return m_PhysicsWorld; }

	private:
		static CoreSystems* m_Instance;

		IRenderEngine* m_RenderEngine;
		IPhysicsWorld* m_PhysicsWorld;
	};
}
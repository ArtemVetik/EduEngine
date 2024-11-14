#pragma once
#include <memory>
#include "framework.h"
#include "RenderEngineDefines.h"
#include "../EduMath/SimpleMath.h"
#include "IDebugRendererSystem.h"
#include "Camera.h"
#include "Window.h"

namespace EduEngine
{
	class RENDERENGINE_API IRenderObject
	{
	public:
		DirectX::SimpleMath::Matrix WorldMatrix;
	};

	class RENDERENGINE_API IRenderEngine
	{
	public:
		virtual IRenderObject* AddObject(NativeMeshData meshData) = 0;
		virtual void RemoveObject(IRenderObject* object) = 0;
		virtual Camera* CreateCamera() = 0;
		virtual void RemoveCamera(Camera* camera) = 0;
		virtual IDebugRendererSystem* GetDebugRender() const = 0;

		virtual void Draw() = 0;

		static std::shared_ptr<IRenderEngine> Create(const Window& mainWindow);
	};
}
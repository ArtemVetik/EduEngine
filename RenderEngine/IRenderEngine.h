#pragma once
#include <SimpleMath.h>
#include <memory>
#include "framework.h"
#include "RenderEngineDefines.h"
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
		virtual std::shared_ptr<IRenderObject> AddObject(MeshData meshData) = 0;
		virtual void RemoveObject(std::shared_ptr<IRenderObject> object) = 0;

		virtual void SetCamera(Camera* camera) = 0;
		virtual IDebugRendererSystem* GetDebugRender() const = 0;

		virtual void Draw() = 0;

		static std::shared_ptr<IRenderEngine> Create(const Window& mainWindow);
	};
}
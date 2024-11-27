#pragma once
#include <memory>
#include "framework.h"
#include "RenderEngineDefines.h"
#include "../EduMath/SimpleMath.h"
#include "imgui/imgui.h"
#include "IDebugRendererSystem.h"
#include "Camera.h"
#include "RuntimeWindow.h"
#include "EditorWindow.h"

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

		virtual void BeginDraw() = 0;
		virtual void Draw(Camera* camera) = 0;
		virtual void EndDraw() = 0;
		virtual void MoveAndResize(UINT x, UINT y, UINT w, UINT h) = 0;

		virtual DirectX::SimpleMath::Vector2 GetScreenSize() const = 0;
		virtual IDebugRendererSystem* GetDebugRender() const = 0;

		static std::shared_ptr<IRenderEngine> Create(const RuntimeWindow& mainWindow);
	};

	class RENDERENGINE_API IEditorRenderEngine
	{
	public:
		virtual void Draw() = 0;
		virtual void UpdateEditor(ImDrawData* drawData) = 0;
		virtual void* CreateEditorImGuiUI(void* pixels, int texWidth, int texHeight, int bytesPerPixel) = 0;
		virtual DirectX::SimpleMath::Vector2 GetScreenSize() const = 0;

		static std::shared_ptr<IEditorRenderEngine> Create(const EditorWindow& mainWindow);
	};
}
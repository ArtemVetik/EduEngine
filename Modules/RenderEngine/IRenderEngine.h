#pragma once
#include <memory>
#include "framework.h"
#include "RenderEngineDefines.h"
#include "../EduMath/SimpleMath.h"
#include "imgui/imgui.h"
#include "IDebugRendererSystem.h"
#include "Timer.h"
#include "Camera.h"
#include "RuntimeWindow.h"
#include "EditorWindow.h"

namespace EduEngine
{
	class RENDERENGINE_API IMesh
	{
	public:
		virtual void UpdateFilePath(const char* filePath) = 0;
		virtual int GetVertexCount() = 0;
		virtual int GetIndexCount() = 0;
	};

	class RENDERENGINE_API ITexture
	{
	public:
		virtual void UpdateFilePath(const wchar_t* filePath) = 0;
		virtual void* GetGPUPtr() = 0;
	};

	class RENDERENGINE_API IMaterial
	{
	public:
		DirectX::XMFLOAT4 DiffuseAlbedo;
		DirectX::XMFLOAT3 FresnelR0;
		float Roughness;

		virtual void SetMainTexture(ITexture* texture) = 0;
		virtual ITexture* GetMainTexture() = 0;
	};

	class RENDERENGINE_API IRenderObject
	{
	public:
		DirectX::SimpleMath::Matrix WorldMatrix = DirectX::SimpleMath::Matrix::Identity;

		virtual void SetMesh(IMesh* mesh) = 0;
		virtual void SetMaterial(IMaterial* material) = 0;
	};

	class RENDERENGINE_API Light
	{
	public:
		enum RENDERENGINE_API Type
		{
			Directional = 0,
			Point = 1,
			Spotlight = 2
		};

		Type LightType = Type::Directional;
		DirectX::XMFLOAT3 Padding = { 0, 0, 0 };
		DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
		float FalloffStart = 1.04f;							 // point/spot light only
		DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f }; // directional/spot light only
		float FalloffEnd = 10.0f;							 // point/spot light only
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };	 // point/spot light only
		float SpotPower = 64.0f;							 // spot light only
	};

	class RENDERENGINE_API IParticleSystem
	{
	public:
		virtual void SetMaxParticles(UINT num) = 0;

		float EmissionRate = 0.0f;
		float LifeTime;
		DirectX::XMFLOAT3 CenterPos;
		DirectX::XMFLOAT4 StartColor;
		DirectX::XMFLOAT4 EndColor;
		DirectX::XMFLOAT3 Velocity;
		DirectX::XMFLOAT3 Acceleration;
	};

	class RENDERENGINE_API IRenderEngine
	{
	public:
		virtual IRenderObject* AddObject() = 0;
		virtual void RemoveObject(IRenderObject* object) = 0;
		virtual IMesh* CreateMesh(const char* filePath) = 0;
		virtual void RemoveMesh(IMesh* mesh) = 0;
		virtual ITexture* CreateTexture(const wchar_t* filePath) = 0;
		virtual void RemoveTexture(ITexture* texture) = 0;
		virtual IMaterial* CreateMaterial() = 0;
		virtual void RemoveMaterial(IMaterial* material) = 0;
		virtual Camera* CreateCamera() = 0;
		virtual void RemoveCamera(Camera* camera) = 0;
		virtual Light* CreateLight() = 0;
		virtual void RemoveLight(Light* light) = 0;
		virtual IParticleSystem* CreateParticleSystem() = 0;
		virtual void RemoveParticleSystem(IParticleSystem* particleSystem) = 0;

		virtual void UpdateUI(ImDrawData* drawData) = 0;
		virtual void* CreateImGuiUI(void* pixels, int texWidth, int texHeight, int bytesPerPixel) = 0;

		virtual void BeginDraw() = 0;
		virtual void Draw(Camera* camera) = 0;
		virtual void EndDraw() = 0;
		virtual void MoveAndResize(UINT x, UINT y, UINT w, UINT h) = 0;
		virtual DirectX::SimpleMath::Vector2 GetScreenSize() const = 0;
		virtual IDebugRendererSystem* GetDebugRender() const = 0;

		static std::shared_ptr<IRenderEngine> Create(const RuntimeWindow& mainWindow, const Timer& timer);
	};

	class RENDERENGINE_API IEditorRenderEngine
	{
	public:
		struct RENDERENGINE_API PreviewMeshInfo
		{
			void* TexturePtr;
			int IndexCount;
			int VertexCount;
		};

		virtual void Draw() = 0;
		virtual void UpdateEditor(ImDrawData* drawData) = 0;
		virtual void* CreateEditorImGuiUI(void* pixels, int texWidth, int texHeight, int bytesPerPixel) = 0;
		virtual DirectX::SimpleMath::Vector2 GetScreenSize() const = 0;

		virtual void* SetPreviewTexture(const wchar_t* filePath) = 0;
		virtual PreviewMeshInfo SetPreviewMesh(const char* filePath) = 0;
		virtual void RotatePreviewMesh(DirectX::XMFLOAT3 delta) = 0;

		static std::shared_ptr<IEditorRenderEngine> Create(const EditorWindow& mainWindow);
	};

	class RENDERENGINE_API IRenderResourcesInfo
	{
	public:
		virtual const char* GetObjectsInfo(bool onlyUsed = false) = 0;

		static std::shared_ptr<IRenderResourcesInfo> Create(IRenderEngine* renderEngine, IEditorRenderEngine* editorRenderEngine);
	};
}
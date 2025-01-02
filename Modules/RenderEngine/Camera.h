#pragma once
#include "framework.h"

using namespace DirectX;

namespace EduEngine
{
	class ITexture;

	class RENDERENGINE_API Camera
	{
	public:
		virtual void SetProjectionMatrix(UINT newWidth, UINT newHeight) = 0;
		virtual void SetProjectionMatrix(float* fov = nullptr, float* nearView = nullptr, float* farView = nullptr) = 0;

		virtual void Update(DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 pos) = 0;
		virtual void SetViewport(XMFLOAT4 viewport) = 0;
		virtual void SetBackgroundColor(XMFLOAT4 color) = 0;
		virtual void SetDebugRenderEnable(bool value) = 0;
		virtual void SetSkybox(ITexture* texture) = 0;

		virtual XMFLOAT4X4 GetViewMatrix() const = 0;
		virtual XMFLOAT4X4 GetProjectionMatrix() const = 0;
		virtual XMMATRIX GetViewProjMatrix() const = 0;
		virtual XMFLOAT3 GetPosition() const = 0;
		virtual XMFLOAT4 GetViewport() const = 0;
		virtual XMFLOAT4 GetBackgroundColor() const = 0;
		virtual float GetNear() const = 0;
		virtual float GetFar() const = 0;
		virtual float GetFovY() const = 0;
		virtual float GetFovX() const = 0;
		virtual bool DebugRenderEnabled() const = 0;
		virtual void* GetSkyGPUPtr() const = 0;
	};
}
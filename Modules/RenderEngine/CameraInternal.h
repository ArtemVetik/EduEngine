#pragma once
#include "Camera.h"
#include "TextureD3D12Impl.h"

using namespace DirectX;

namespace EduEngine
{
	class CameraInternal : public Camera
	{
	public:
		CameraInternal(RenderDeviceD3D12* device, UINT width, UINT height);

		void SetProjectionMatrix(UINT newWidth, UINT newHeight) override;
		void SetProjectionMatrix(float* fov = nullptr, float* nearView = nullptr, float* farView = nullptr) override;

		void Update(DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 pos) override;
		void SetViewport(XMFLOAT4 viewport) override;
		void SetBackgroundColor(XMFLOAT4 color) override;
		void SetDebugRenderEnable(bool value) override;
		void SetSkybox(ITexture* texture) override;

		XMFLOAT4X4 GetViewMatrix() const override;
		XMFLOAT4X4 GetProjectionMatrix() const override;
		XMMATRIX GetViewProjMatrix() const override;
		XMFLOAT3 GetPosition() const override;
		XMFLOAT4 GetViewport() const override;
		XMFLOAT4 GetBackgroundColor() const override;
		float GetNear() const override;
		float GetFar() const override;
		float GetFovY() const override;
		float GetFovX() const override;
		bool DebugRenderEnabled() const override;
		void* GetSkyGPUPtr() const override;

	private:
		RenderDeviceD3D12* m_Device;

		UINT m_ScreenWidth;
		UINT m_ScreenHeight;

		float m_FovY = 55.0f * (3.14f / 180.0f);
		float m_FovX;

		XMFLOAT4 m_Viewport;
		XMFLOAT4 m_BackgroundColor;
		XMFLOAT4X4 m_ViewMatrix;
		XMFLOAT4X4 m_ProjectionMatrix;
		float m_NearValue;
		float m_FarValue;

		XMFLOAT3 m_Position;

		bool m_DebugRenderEnabled;
		TextureD3D12Impl* m_SkyboxTex;
	};
}
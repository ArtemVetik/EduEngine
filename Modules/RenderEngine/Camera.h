#pragma once
#include "framework.h"
#include "../EduMath/SimpleMath.h"

using namespace DirectX;

namespace EduEngine
{
	class RENDERENGINE_API Camera
	{
	public:
		Camera(UINT width, UINT height);

		void SetProjectionMatrix(UINT newWidth, UINT newHeight);
		void SetProjectionMatrix(float* fov = nullptr, float* nearView = nullptr, float* farView = nullptr);

		void Update(DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 pos);
		void SetViewport(XMFLOAT4 viewport);
		void SetBackgroundColor(XMFLOAT4 color);
		void SetDebugRenderEnable(bool value);

		XMFLOAT4X4 GetViewMatrix() const;
		XMFLOAT4X4 GetProjectionMatrix() const;
		XMMATRIX GetViewProjMatrix() const;
		XMFLOAT3 GetPosition() const;
		XMFLOAT4 GetViewport() const;
		XMFLOAT4 GetBackgroundColor() const;
		float GetNear() const;
		float GetFar() const;
		float GetFovY() const;
		float GetFovX() const;
		bool DebugRenderEnabled() const;

	private:
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
	};
}
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

		XMFLOAT4X4 GetViewMatrix() const;
		XMFLOAT4X4 GetProjectionMatrix() const;
		XMMATRIX GetViewProjMatrix() const;
		XMFLOAT3 GetPosition() const;
		XMFLOAT4 GetViewport() const;
		float GetNear() const;
		float GetFar() const;
		float GetFov() const;

	private:
		UINT m_ScreenWidth;
		UINT m_ScreenHeight;

		float m_Fov = 55.0f;

		XMFLOAT4 m_Viewport;
		XMFLOAT4X4 m_ViewMatrix;
		XMFLOAT4X4 m_ProjectionMatrix;
		float m_NearValue;
		float m_FarValue;
		
		XMFLOAT3 m_Position;
	};
}
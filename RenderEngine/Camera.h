#pragma once
#include "framework.h"
#include <d3d12.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace EduEngine
{
	class RENDERENGINE_API Camera
	{
	public:
		Camera(UINT width, UINT height);

		void SetProjectionMatrix(UINT newWidth, UINT newHeight);

		void Update(DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 pos);

		XMFLOAT4X4 GetViewMatrix() const;
		XMFLOAT4X4 GetProjectionMatrix() const;
		XMMATRIX GetViewProjMatrix() const;
		XMFLOAT3 GetPosition();
		float GetNear() const;
		float GetFar() const;
		float GetFov() const;

	private:
		UINT m_ScreenWidth;
		UINT m_ScreenHeight;

		float m_Fov = 55.0f;

		XMFLOAT4X4 m_ViewMatrix;
		XMFLOAT4X4 m_ProjectionMatrix;
		float m_NearValue;
		float m_FarValue;

		XMFLOAT3 m_Position;
	};
}
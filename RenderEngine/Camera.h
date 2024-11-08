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

		void Shift(XMVECTOR shift);
		void Pitch(float angle);
		void RotateY(float angle);

		void SetProjectionMatrix(UINT newWidth, UINT newHeight);

		void Update();
		void ResetCamera();

		XMFLOAT4X4 GetViewMatrix() const;
		XMFLOAT4X4 GetProjectionMatrix() const;
		XMMATRIX GetViewProjMatrix() const;
		XMFLOAT3 GetPosition() const;
		XMFLOAT3 GetLook();
		XMFLOAT3 GetRight();
		XMFLOAT3 GetUp();
		float GetNear() const;
		float GetFar() const;
		float GetFov() const;

	private:
		UINT m_ScreenWidth;
		UINT m_ScreenHeight;

		XMFLOAT3 m_Position = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 m_Right = { 1.0f, 0.0f, 0.0f };
		XMFLOAT3 m_Up = { 0.0f, 1.0f, 0.0f };
		XMFLOAT3 m_Look = { 0.0f, 0.0f, 1.0f };
		float m_Fov = 55.0f;

		XMFLOAT4X4 m_ViewMatrix;
		XMFLOAT4X4 m_ProjectionMatrix;
		float m_NearValue;
		float m_FarValue;

		bool m_ViewDirty = true;
	};
}
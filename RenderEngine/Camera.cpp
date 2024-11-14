#include "pch.h"
#include "Camera.h"

namespace EduEngine
{
	Camera::Camera(UINT width, UINT height)
	{
		m_NearValue = 0.3f;
		m_FarValue = 1000.0f;

		XMVECTOR pos = XMVectorSet(0.0f, 10.0f, -20.0f, 0.0f);
		XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);

		XMMATRIX V = XMMatrixLookToLH(
			pos,
			dir,
			up);
		XMStoreFloat4x4(&m_ViewMatrix, (V));

		SetProjectionMatrix(width, height);

		XMFLOAT3 fLook;
		XMFLOAT3 fUp;
		XMFLOAT3 fRight;

		XMStoreFloat3(&fLook, dir);
		XMStoreFloat3(&fUp, up);
		XMStoreFloat3(&fRight, XMVector3Cross(up, dir));
		XMStoreFloat3(&m_Position, pos);

		Update(fLook, fRight, fUp, m_Position);
	}

	XMFLOAT4X4 Camera::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	XMFLOAT4X4 Camera::GetProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	XMMATRIX Camera::GetViewProjMatrix() const
	{
		return DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix);
	}

	XMFLOAT3 Camera::GetPosition()
	{
		return m_Position;
	}

	float Camera::GetNear() const
	{
		return m_NearValue;
	}

	float Camera::GetFar() const
	{
		return m_FarValue;
	}

	float Camera::GetFov() const
	{
		return m_Fov;
	}

	void Camera::SetProjectionMatrix(UINT newWidth, UINT newHeight)
	{
		XMMATRIX P = XMMatrixPerspectiveFovLH(m_Fov * (3.14f / 180.0f), (float)newWidth / (float)newHeight, m_NearValue, m_FarValue);
		XMStoreFloat4x4(&m_ProjectionMatrix, (P));
	}

	void Camera::Update(DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 pos)
	{
		m_Position = pos;

		XMVECTOR R = XMLoadFloat3(&right);
		XMVECTOR U = XMLoadFloat3(&up);
		XMVECTOR L = XMLoadFloat3(&look);
		XMVECTOR P = XMLoadFloat3(&pos);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&right, R);
		XMStoreFloat3(&up, U);
		XMStoreFloat3(&look, L);

		m_ViewMatrix(0, 0) = right.x;
		m_ViewMatrix(1, 0) = right.y;
		m_ViewMatrix(2, 0) = right.z;
		m_ViewMatrix(3, 0) = x;

		m_ViewMatrix(0, 1) = up.x;
		m_ViewMatrix(1, 1) = up.y;
		m_ViewMatrix(2, 1) = up.z;
		m_ViewMatrix(3, 1) = y;

		m_ViewMatrix(0, 2) = look.x;
		m_ViewMatrix(1, 2) = look.y;
		m_ViewMatrix(2, 2) = look.z;
		m_ViewMatrix(3, 2) = z;

		m_ViewMatrix(0, 3) = 0.0f;
		m_ViewMatrix(1, 3) = 0.0f;
		m_ViewMatrix(2, 3) = 0.0f;
		m_ViewMatrix(3, 3) = 1.0f;
	}
}

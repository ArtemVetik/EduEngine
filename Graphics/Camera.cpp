#include "pch.h"
#include "Camera.h"

namespace EduEngine
{
	Camera::Camera(UINT width, UINT height)
	{
		ResetCamera();

		m_NearValue = 5.0f;
		m_FarValue = 1000.0f;

		XMVECTOR pos = XMVectorSet(0.0f, 20.0f, -150.0f, 0.0f);
		XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);

		XMMATRIX V = XMMatrixLookToLH(
			pos,
			dir,
			up);
		XMStoreFloat4x4(&m_ViewMatrix, (V));

		SetProjectionMatrix(width, height);

		XMStoreFloat3(&m_Position, pos);
		XMStoreFloat3(&m_Look, dir);
		XMStoreFloat3(&m_Up, up);
		XMStoreFloat3(&m_Right, XMVector3Cross(up, dir));
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

	DirectX::XMFLOAT3 Camera::GetPosition() const
	{
		return m_Position;
	}

	XMFLOAT3 Camera::GetLook()
	{
		return m_Look;
	}

	XMFLOAT3 Camera::GetRight()
	{
		return m_Right;
	}

	XMFLOAT3 Camera::GetUp()
	{
		return m_Up;
	}

	void Camera::Shift(XMVECTOR shift)
	{
		XMVECTOR pos = XMLoadFloat3(&m_Position);
		pos += shift;
		XMStoreFloat3(&m_Position, pos);

		m_ViewDirty = true;
	}

	void Camera::Pitch(float angle)
	{
		// Rotate up and look vector about the right vector.

		XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

		XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));

		m_ViewDirty = true;
	}

	void Camera::RotateY(float angle)
	{
		// Rotate the basis vectors about the world y-axis.

		XMMATRIX R = XMMatrixRotationY(angle);

		XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
		XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));

		m_ViewDirty = true;
	}

	void Camera::SetProjectionMatrix(UINT newWidth, UINT newHeight)
	{
		XMMATRIX P = XMMatrixPerspectiveFovLH(m_Fov * (3.14f / 180.0f), (float)newWidth / (float)newHeight, m_NearValue, m_FarValue);
		XMStoreFloat4x4(&m_ProjectionMatrix, (P));
	}

	void Camera::Update()
	{
		if (m_ViewDirty)
		{
			XMVECTOR R = XMLoadFloat3(&m_Right);
			XMVECTOR U = XMLoadFloat3(&m_Up);
			XMVECTOR L = XMLoadFloat3(&m_Look);
			XMVECTOR P = XMLoadFloat3(&m_Position);

			// Keep camera's axes orthogonal to each other and of unit length.
			L = XMVector3Normalize(L);
			U = XMVector3Normalize(XMVector3Cross(L, R));

			// U, L already ortho-normal, so no need to normalize cross product.
			R = XMVector3Cross(U, L);

			// Fill in the view matrix entries.
			float x = -XMVectorGetX(XMVector3Dot(P, R));
			float y = -XMVectorGetX(XMVector3Dot(P, U));
			float z = -XMVectorGetX(XMVector3Dot(P, L));

			XMStoreFloat3(&m_Right, R);
			XMStoreFloat3(&m_Up, U);
			XMStoreFloat3(&m_Look, L);

			m_ViewMatrix(0, 0) = m_Right.x;
			m_ViewMatrix(1, 0) = m_Right.y;
			m_ViewMatrix(2, 0) = m_Right.z;
			m_ViewMatrix(3, 0) = x;

			m_ViewMatrix(0, 1) = m_Up.x;
			m_ViewMatrix(1, 1) = m_Up.y;
			m_ViewMatrix(2, 1) = m_Up.z;
			m_ViewMatrix(3, 1) = y;

			m_ViewMatrix(0, 2) = m_Look.x;
			m_ViewMatrix(1, 2) = m_Look.y;
			m_ViewMatrix(2, 2) = m_Look.z;
			m_ViewMatrix(3, 2) = z;

			m_ViewMatrix(0, 3) = 0.0f;
			m_ViewMatrix(1, 3) = 0.0f;
			m_ViewMatrix(2, 3) = 0.0f;
			m_ViewMatrix(3, 3) = 1.0f;

			m_ViewDirty = false;
		}
	}

	void Camera::ResetCamera()
	{
		XMVECTOR pos = XMVectorSet(0.0f, -10.0f, -150.0f, 0.0f);
		XMStoreFloat3(&m_Position, pos);
	}
}
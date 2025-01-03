#include "pch.h"
#include "IRenderEngine.h"
#include "CameraInternal.h"

namespace EduEngine
{
	CameraInternal::CameraInternal(RenderDeviceD3D12* device, UINT width, UINT height) :
		m_Device(device),
		m_SkyboxTex(nullptr)
	{
		m_NearValue = 0.3f;
		m_FarValue = 1000.0f;
		m_Viewport = { 0, 0, 1, 1 };
		m_BackgroundColor = { 0, 0, 0, 1 };
		m_DebugRenderEnabled = false;

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

	XMFLOAT4X4 CameraInternal::GetViewMatrix() const
	{
		return m_ViewMatrix;
	}

	XMFLOAT4X4 CameraInternal::GetProjectionMatrix() const
	{
		return m_ProjectionMatrix;
	}

	XMMATRIX CameraInternal::GetViewProjMatrix() const
	{
		return DirectX::XMLoadFloat4x4(&m_ViewMatrix) * DirectX::XMLoadFloat4x4(&m_ProjectionMatrix);
	}

	XMFLOAT3 CameraInternal::GetPosition() const
	{
		return m_Position;
	}

	XMFLOAT4 CameraInternal::GetViewport() const
	{
		return m_Viewport;
	}

	XMFLOAT4 CameraInternal::GetBackgroundColor() const
	{
		return m_BackgroundColor;
	}

	float CameraInternal::GetNear() const
	{
		return m_NearValue;
	}

	float CameraInternal::GetFar() const
	{
		return m_FarValue;
	}

	float CameraInternal::GetFovY() const
	{
		return m_FovY;
	}

	float CameraInternal::GetFovX() const
	{
		return m_FovX;
	}

	bool CameraInternal::DebugRenderEnabled() const
	{
		return m_DebugRenderEnabled;
	}

	void* CameraInternal::GetSkyGPUPtr() const
	{
		if (m_SkyboxTex)
			return m_SkyboxTex->GetGPUPtr();

		return nullptr;
	}

	void CameraInternal::SetProjectionMatrix(UINT newWidth, UINT newHeight)
	{
		m_ScreenWidth = newWidth;
		m_ScreenHeight = newHeight;

		SetProjectionMatrix();
	}

	void CameraInternal::SetProjectionMatrix(float* fov, float* nearView, float* farView)
	{
		if (fov) m_FovY = *fov;
		if (nearView) m_NearValue = *nearView;
		if (farView) m_FarValue = *farView;

		m_FovY = max(m_FovY, FLT_MIN);
		m_NearValue = max(m_NearValue, FLT_MIN);
		m_FarValue = max(m_FarValue, m_NearValue + 0.1f);

		auto aspectRatio = (float)m_ScreenWidth / (float)m_ScreenHeight;
		auto nearWindowHeight = 2.0f * m_NearValue * tanf(0.5f * m_FovY);
		float halfWidth = 0.5f * aspectRatio * nearWindowHeight;
		m_FovX = 2.0f * atan(halfWidth / m_NearValue);

		XMMATRIX P = XMMatrixPerspectiveFovLH(
			m_FovY,
			aspectRatio,
			m_NearValue,
			m_FarValue
		);
		XMStoreFloat4x4(&m_ProjectionMatrix, (P));
	}

	void CameraInternal::Update(DirectX::XMFLOAT3 look, DirectX::XMFLOAT3 right, DirectX::XMFLOAT3 up, DirectX::XMFLOAT3 pos)
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

	void CameraInternal::SetViewport(XMFLOAT4 viewport)
	{
		m_Viewport = viewport;
	}

	void CameraInternal::SetBackgroundColor(XMFLOAT4 color)
	{
		m_BackgroundColor = color;
	}

	void CameraInternal::SetDebugRenderEnable(bool value)
	{
		m_DebugRenderEnabled = value;
	}

	void CameraInternal::SetSkybox(ITexture* texture)
	{
		if (m_SkyboxTex)
			m_SkyboxTex->Free();

		m_SkyboxTex = dynamic_cast<TextureD3D12Impl*>(texture);

		if (m_SkyboxTex)
			m_SkyboxTex->Load();
	}
}

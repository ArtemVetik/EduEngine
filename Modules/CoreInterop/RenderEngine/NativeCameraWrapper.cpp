#include "NativeCameraWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	NativeCameraWrapper::NativeCameraWrapper()
	{
		m_NativeCamera = CoreSystems::GetInstance()->GetRenderEngine()->CreateCamera();;
	}

	NativeCameraWrapper::~NativeCameraWrapper()
	{
		this->!NativeCameraWrapper();
	}

	NativeCameraWrapper::!NativeCameraWrapper()
	{
		if (m_NativeCamera)
		{
			CoreSystems::GetInstance()->GetRenderEngine()->RemoveCamera(m_NativeCamera);
			m_NativeCamera = nullptr;
		}
	}

	void NativeCameraWrapper::Update(Vector3 look, Vector3 right, Vector3 up, Vector3 pos)
	{
		m_NativeCamera->Update(
			{ look.X, look.Y, look.Z },
			{ right.X, right.Y, right.Z },
			{ up.X, up.Y, up.Z },
			{ pos.X, pos.Y, pos.Z }
		);
	}
	void NativeCameraWrapper::Render()
	{
		if (!m_NativeCamera)
			return;

		CoreSystems::GetInstance()->GetRenderEngine()->Draw(m_NativeCamera);
	}

	void NativeCameraWrapper::DrawFrustrum()
	{
		if (!m_NativeCamera)
			return;

		NativeCameraWrapperUnmanaged::DrawFrustrum(m_NativeCamera);
	}

	void NativeCameraWrapper::SetProjectionMatrix(float fov, float nearView, float farView)
	{
		fov = fov * 3.14f / 180.0f;
		m_NativeCamera->SetProjectionMatrix(&fov, &nearView, &farView);
	}

	void NativeCameraWrapper::SetViewport(Vector4 viewport)
	{
		m_NativeCamera->SetViewport({ viewport.X,viewport.Y, viewport.Z, viewport.W });
	}

	void NativeCameraWrapper::SetBackgroundColor(Vector4 color)
	{
		m_NativeCamera->SetBackgroundColor({ color.X,color.Y, color.Z, color.W });
	}

	void NativeCameraWrapper::SetDebugRenderEnable(bool value)
	{
		m_NativeCamera->SetDebugRenderEnable(value);
	}

	void NativeCameraWrapper::SetSkybox(TextureWrapper^ texture)
	{
		if (texture == nullptr)
			m_NativeCamera->SetSkybox(nullptr);
		else
			m_NativeCamera->SetSkybox(texture->GetNative());
	}

	System::Numerics::Matrix4x4 NativeCameraWrapper::GetView()
	{
		auto nativeMatrix = m_NativeCamera->GetViewMatrix();
		return System::Numerics::Matrix4x4(
			nativeMatrix.m[0][0], nativeMatrix.m[0][1], nativeMatrix.m[0][2], nativeMatrix.m[0][3],
			nativeMatrix.m[1][0], nativeMatrix.m[1][1], nativeMatrix.m[1][2], nativeMatrix.m[1][3],
			nativeMatrix.m[2][0], nativeMatrix.m[2][1], nativeMatrix.m[2][2], nativeMatrix.m[2][3],
			nativeMatrix.m[3][0], nativeMatrix.m[3][1], nativeMatrix.m[3][2], nativeMatrix.m[3][3]
		);
	}

	System::Numerics::Matrix4x4 NativeCameraWrapper::GetProjection()
	{
		auto nativeMatrix = m_NativeCamera->GetProjectionMatrix();
		return System::Numerics::Matrix4x4(
			nativeMatrix.m[0][0], nativeMatrix.m[0][1], nativeMatrix.m[0][2], nativeMatrix.m[0][3],
			nativeMatrix.m[1][0], nativeMatrix.m[1][1], nativeMatrix.m[1][2], nativeMatrix.m[1][3],
			nativeMatrix.m[2][0], nativeMatrix.m[2][1], nativeMatrix.m[2][2], nativeMatrix.m[2][3],
			nativeMatrix.m[3][0], nativeMatrix.m[3][1], nativeMatrix.m[3][2], nativeMatrix.m[3][3]
		);
	}

	Vector4 NativeCameraWrapper::GetViewport()
	{
		auto nativeViewport = m_NativeCamera->GetViewport();
		return Vector4(nativeViewport.x, nativeViewport.y, nativeViewport.z, nativeViewport.w);
	}

	Vector4 NativeCameraWrapper::GetBackgroundColor()
	{
		auto backgroundColor = m_NativeCamera->GetBackgroundColor();
		return Vector4(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	}

	bool NativeCameraWrapper::IsDebugRenderEnabled()
	{
		return m_NativeCamera->DebugRenderEnabled();
	}

	float NativeCameraWrapper::GetFov()
	{
		return m_NativeCamera->GetFovY();
	}

	float NativeCameraWrapper::GetNear()
	{
		return m_NativeCamera->GetNear();
	}

	float NativeCameraWrapper::GetFar()
	{
		return m_NativeCamera->GetFar();
	}

	void NativeCameraWrapperUnmanaged::DrawFrustrum(Camera* nativeCamera)
	{
		auto view = DirectX::XMLoadFloat4x4(&nativeCamera->GetViewMatrix());
		auto proj = DirectX::XMLoadFloat4x4(&nativeCamera->GetProjectionMatrix());
		CoreSystems::GetInstance()->GetRenderEngine()->GetDebugRender()->DrawFrustrum(view, proj);
	}
}
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

	void NativeCameraWrapper::DebugRender()
	{
		if (!m_NativeCamera)
			return;

		CoreSystems::GetInstance()->GetRenderEngine()->DebugDraw(m_NativeCamera);
	}

	void NativeCameraWrapper::DrawFrustrum()
	{
		if (!m_NativeCamera)
			return;

		NativeCameraWrapperUnmanaged::DrawFrustrum(m_NativeCamera);
	}

	void NativeCameraWrapper::SetProjectionMatrix(float fov, float nearView, float farView)
	{
		m_NativeCamera->SetProjectionMatrix(&fov, &nearView, &farView);
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

	float NativeCameraWrapper::GetFov()
	{
		return m_NativeCamera->GetFov();
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
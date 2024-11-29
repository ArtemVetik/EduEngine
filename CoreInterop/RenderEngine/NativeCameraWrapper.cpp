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
		CoreSystems::GetInstance()->GetRenderEngine()->Draw(m_NativeCamera);
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
}
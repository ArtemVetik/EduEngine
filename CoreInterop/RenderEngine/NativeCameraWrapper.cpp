#include "NativeCameraWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	NativeCameraWrapper::NativeCameraWrapper(Camera* nativeCamera)
	{
		m_NativeCamera = nativeCamera;
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
}
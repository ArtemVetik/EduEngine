#include "NativeRenderObjectWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	NativeRenderObjectWrapper::NativeRenderObjectWrapper(MeshData^ meshData)
	{
		m_NativeObject = CoreSystems::GetInstance()->GetRenderEngine()->AddObject(meshData->ToNative());
	}

	NativeRenderObjectWrapper::~NativeRenderObjectWrapper()
	{
		this->!NativeRenderObjectWrapper();
	}

	NativeRenderObjectWrapper::!NativeRenderObjectWrapper()
	{
		if (m_NativeObject != nullptr)
		{
			CoreSystems::GetInstance()->GetRenderEngine()->RemoveObject(m_NativeObject);
			m_NativeObject = nullptr;
		}
	}

	void NativeRenderObjectWrapper::SetWorldMatrix(Matrix4x4 matrix)
	{
		m_NativeObject->WorldMatrix = DirectX::SimpleMath::Matrix(
			matrix.M11, matrix.M12, matrix.M13, matrix.M14,
			matrix.M21, matrix.M22, matrix.M23, matrix.M24,
			matrix.M31, matrix.M32, matrix.M33, matrix.M34,
			matrix.M41, matrix.M42, matrix.M43, matrix.M44
		);
	}
}
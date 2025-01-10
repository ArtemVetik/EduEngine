#include "NativeRenderObjectWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	NativeRenderObjectWrapper::NativeRenderObjectWrapper()
	{
		m_NativeObject = CoreSystems::GetInstance()->GetRenderEngine()->AddObject();
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

	void NativeRenderObjectWrapper::SetTextureTransform(Matrix4x4 matrix)
	{
		m_NativeObject->TextureTransform = DirectX::SimpleMath::Matrix(
			matrix.M11, matrix.M12, matrix.M13, matrix.M14,
			matrix.M21, matrix.M22, matrix.M23, matrix.M24,
			matrix.M31, matrix.M32, matrix.M33, matrix.M34,
			matrix.M41, matrix.M42, matrix.M43, matrix.M44
		);
	}

	void NativeRenderObjectWrapper::SetMesh(SharedMeshWrapper^ mesh)
	{
		if (mesh == nullptr)
			m_NativeObject->SetMesh(nullptr);
		else
			m_NativeObject->SetMesh(mesh->GetNative());
	}

	void NativeRenderObjectWrapper::SetMaterial(MaterialWrapper^ material)
	{
		if (material == nullptr)
			m_NativeObject->SetMaterial(nullptr);
		else
			m_NativeObject->SetMaterial(material->GetNative());
	}
}
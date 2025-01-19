#include "pch.h"
#include "RenderObject.h"

namespace EduEngine
{
	RenderObject::RenderObject() :
		m_Mesh(nullptr),
		m_Material(nullptr)
	{
	}

	RenderObject::~RenderObject()
	{
		if (m_Mesh)
			m_Mesh->Free();
	}

	void RenderObject::SetMesh(IMesh* mesh)
	{
		if (m_Mesh == mesh)
			return;

		if (m_Mesh)
			m_Mesh->Free();

		m_Mesh = dynamic_cast<SharedMeshD3D12Impl*>(mesh);

		if (m_Mesh)
			m_Mesh->Load();
	}

	void RenderObject::SetMaterial(IMaterial* material)
	{
		if (m_Material == material)
			return;

		if (m_Material)
			m_Material->Free();

		m_Material = dynamic_cast<MaterialD3D12Impl*>(material);

		if (m_Material)
			m_Material->Load();
	}

	VertexBufferD3D12* RenderObject::GetVertexBuffer() const
	{
		if (m_Mesh)
			return m_Mesh->GetVertexBuffer();

		return nullptr;
	}

	IndexBufferD3D12* RenderObject::GetIndexBuffer() const
	{
		if (m_Mesh)
			return m_Mesh->GetIndexBuffer();

		return nullptr;
	}
}
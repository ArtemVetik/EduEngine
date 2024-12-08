#include "pch.h"
#include "RenderObject.h"

namespace EduEngine
{
	RenderObject::RenderObject(SharedMeshD3D12Impl* mesh) :
		m_Mesh(mesh)
	{
		m_Mesh->Load();
	}

	RenderObject::~RenderObject()
	{
		if (m_Mesh)
			m_Mesh->Free();
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

	void RenderObject::RemoveMesh(SharedMeshD3D12Impl* mesh)
	{
		if (m_Mesh == mesh)
			m_Mesh = nullptr;
	}
}
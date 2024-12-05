#include "pch.h"
#include "SharedMeshD3D12Impl.h"

namespace EduEngine
{
	SharedMeshD3D12Impl::SharedMeshD3D12Impl(RenderDeviceD3D12* device, const aiScene* scene) :
		m_Device(device),
		m_Scene(scene),
		m_RefCount(0)
	{
	}

	void SharedMeshD3D12Impl::Load()
	{
		if (m_RefCount > 0)
		{
			m_RefCount++;
			return;
		}

		NativeMeshData meshData;
		for (int i = 0; i < m_Scene->mMeshes[0]->mNumVertices; i++)
		{
			auto aiVertex = m_Scene->mMeshes[0]->mVertices[i];
			auto aiNormal = m_Scene->mMeshes[0]->mNormals[i];

			meshData.Vertices.push_back(NativeVertex(
				{ aiVertex.x, aiVertex.y, aiVertex.z }, // p
				{ aiNormal.x, aiNormal.y, aiNormal.z }, // n
				{ }, // t
				{ } // uv
			));
		}

		for (size_t i = 0; i < m_Scene->mMeshes[0]->mNumFaces; i++)
		{
			for (size_t k = 0; k < m_Scene->mMeshes[0]->mFaces[i].mNumIndices; k += 3)
			{
				meshData.Indices32.push_back(m_Scene->mMeshes[0]->mFaces[i].mIndices[k + 2]);
				meshData.Indices32.push_back(m_Scene->mMeshes[0]->mFaces[i].mIndices[k]);
				meshData.Indices32.push_back(m_Scene->mMeshes[0]->mFaces[i].mIndices[k + 1]);
			}
		}

		m_VertexBuffer = std::make_shared<VertexBufferD3D12>(m_Device, meshData.Vertices.data(),
			sizeof(NativeVertex), (UINT)meshData.Vertices.size());
		m_IndexBuffer = std::make_shared<IndexBufferD3D12>(m_Device, meshData.GetIndices16().data(),
			sizeof(uint16), (UINT)meshData.GetIndices16().size(), DXGI_FORMAT_R16_UINT);

		m_RefCount = 1;
	}

	void SharedMeshD3D12Impl::Free()
	{
		if (m_RefCount == 0)
			return;

		m_RefCount--;

		if (m_RefCount == 0)
		{
			m_VertexBuffer.reset();
			m_IndexBuffer.reset();
		}

	}

	int SharedMeshD3D12Impl::GetVertexCount()
	{
		return m_Scene->mMeshes[0]->mNumVertices;
	}

	int SharedMeshD3D12Impl::GetIndexCount()
	{
		return m_Scene->mMeshes[0]->mNumFaces * 3;
	}
}
#pragma once
#include <assimp/scene.h>
#include "IRenderEngine.h"
#include "../Graphics/BufferD3D12.h"

namespace EduEngine
{
	class SharedMeshD3D12Impl : public IMesh
	{
	public:
		SharedMeshD3D12Impl(RenderDeviceD3D12* device, const aiScene* scene);

		void Load();
		void Free();
		
		int GetVertexCount() override;
		int GetIndexCount() override;

		VertexBufferD3D12* GetVertexBuffer() const { return m_VertexBuffer.get(); }
		IndexBufferD3D12* GetIndexBuffer() const { return m_IndexBuffer.get(); }

	private:
		const aiScene* m_Scene;
		RenderDeviceD3D12* m_Device;
		std::shared_ptr<VertexBufferD3D12> m_VertexBuffer;
		std::shared_ptr<IndexBufferD3D12> m_IndexBuffer;

		int m_RefCount;
	};
}
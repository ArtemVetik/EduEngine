#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "IRenderEngine.h"
#include "../Graphics/BufferD3D12.h"

namespace EduEngine
{
	class SharedMeshD3D12Impl : public IMesh
	{
	public:
		SharedMeshD3D12Impl(RenderDeviceD3D12* device, const char* filePath);
		~SharedMeshD3D12Impl();

		void Load();
		void Free();

		void UpdateFilePath(const char* filePath) override;

		int GetRefCount() const { return m_RefCount; }
		const char* GetFilePath() const { return m_FilePath; }

		int GetVertexCount() override;
		int GetIndexCount() override;

		VertexBufferD3D12* GetVertexBuffer() const { return m_VertexBuffer.get(); }
		IndexBufferD3D12* GetIndexBuffer() const { return m_IndexBuffer.get(); }

	private:
		RenderDeviceD3D12* m_Device;
		Assimp::Importer m_AssimpImporter;
		const aiScene* m_Scene;
		std::shared_ptr<VertexBufferD3D12> m_VertexBuffer;
		std::shared_ptr<IndexBufferD3D12> m_IndexBuffer;

		const char* m_FilePath;
		int m_RefCount;
	};
}
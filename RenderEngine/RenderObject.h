#pragma once
#include <memory>
#include "IRenderEngine.h"
#include "SharedMeshD3D12Impl.h"
#include "../Graphics/BufferD3D12.h"

namespace EduEngine
{
	class RenderObject : public IRenderObject
	{
	public:
		RenderObject(SharedMeshD3D12Impl* mesh);
		~RenderObject();
		
		VertexBufferD3D12* GetVertexBuffer() const;
		IndexBufferD3D12* GetIndexBuffer() const;
	
		void RemoveMesh(SharedMeshD3D12Impl* mesh);
	private:
		SharedMeshD3D12Impl* m_Mesh;
	};
}
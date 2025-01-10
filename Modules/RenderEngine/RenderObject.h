#pragma once
#include <memory>
#include "IRenderEngine.h"
#include "SharedMeshD3D12Impl.h"
#include "MaterialD3D12Impl.h"
#include "../Graphics/BufferD3D12.h"

namespace EduEngine
{
	class RenderObject : public IRenderObject
	{
	public:
		RenderObject();
		~RenderObject();
		
		void SetMesh(IMesh* mesh) override;
		void SetMaterial(IMaterial* material) override;

		VertexBufferD3D12* GetVertexBuffer() const;
		IndexBufferD3D12* GetIndexBuffer() const;
	
		IMesh* GetMesh() const override { return m_Mesh; }
		IMaterial* GetMaterial() const override { return m_Material; }

	private:
		SharedMeshD3D12Impl* m_Mesh;
		MaterialD3D12Impl* m_Material;
	};
}
#pragma once
#include "SharedMeshD3D12Impl.h"
#include "TextureD3D12Impl.h"
#include "CameraInternal.h"
#include "RenderPasses.h"

namespace EduEngine
{
	class EditorMeshPreviewD3D12
	{
	public:
		EditorMeshPreviewD3D12(RenderDeviceD3D12* device);
		IEditorRenderEngine::PreviewMeshInfo SetPreviewMesh(const char* filePath);

		void Rotate(DirectX::XMFLOAT3 delta);
		void Render();

		void* GetMeshPtr() const { return m_PreviewMesh.get(); }

	private:
		RenderDeviceD3D12* m_Device;

		std::shared_ptr<SharedMeshD3D12Impl> m_PreviewMesh;
		std::shared_ptr<TextureD3D12> m_PreviewMeshRT;
		std::shared_ptr<TextureD3D12> m_PreviewMeshDSV;

		std::unique_ptr<CameraInternal> m_PreviewCamera;
		std::unique_ptr<OpaquePass> m_OpaquePass;

		float m_CamRadius;
		float m_CamTheta;
		float m_CamPhi;

		int m_Width;
		int m_Height;
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;
	};
}
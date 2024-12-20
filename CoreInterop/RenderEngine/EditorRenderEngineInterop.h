#pragma once
#include "../../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	using namespace System;

	private value class PreviewMeshInfo
	{
	public:
		IntPtr TexturePtr;
		int VertexCount;
		int IndexCount;

		static PreviewMeshInfo FromNative(IEditorRenderEngine::PreviewMeshInfo info);
	};

	private ref class EditorRenderEngineInterop
	{
	public:
		static System::Numerics::Vector2 GetEditorSize();
		static IntPtr CreateImGuiEditor(IntPtr pixels, int texWidth, int texHeight, int bytesPerPixel);
		static void UpdateImGui(IntPtr drawData);

		static IntPtr SetPreviewTexture(String^ filePath);
		static PreviewMeshInfo SetPreviewMesh(String^ filePath);
		static void RotatePreviewMesh(Numerics::Vector3 delta);
	};
}
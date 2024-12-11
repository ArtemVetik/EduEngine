#pragma once

namespace EduEngine
{
	using namespace System;

	private ref class EditorRenderEngineInterop
	{
	public:
		static System::Numerics::Vector2 GetEditorSize();
		static void* CreateImGuiEditor(void* pixels, int texWidth, int texHeight, int bytesPerPixel);
		static void UpdateImGui(void* drawData);

		static IntPtr SetPreviewTexture(String^ filePath);
		static IntPtr SetPreviewMesh(String^ filePath);
		static void RotatePreviewMesh(Numerics::Vector3 delta);
	};
}
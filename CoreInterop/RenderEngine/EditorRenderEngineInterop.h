#pragma once

namespace EduEngine
{
	private ref class EditorRenderEngineInterop
	{
	public:
		static System::Numerics::Vector2 GetEditorSize();
		static void* CreateImGuiEditor(void* pixels, int texWidth, int texHeight, int bytesPerPixel);
		static void UpdateImGui(void* drawData);
	};
}
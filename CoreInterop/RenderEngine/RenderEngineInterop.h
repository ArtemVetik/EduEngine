#pragma once

namespace EduEngine
{
	private ref class RenderEngineInterop
	{
	public:
		static System::Numerics::Vector2 GetScreenSize();
		static void* CreateImGuiEditor(void* pixels, int texWidth, int texHeight, int bytesPerPixel);
		static void UpdateImGui(void* drawData);
	};
}
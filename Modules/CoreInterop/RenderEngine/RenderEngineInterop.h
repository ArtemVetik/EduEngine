#pragma once

namespace EduEngine
{
	using namespace System;
	
	private ref class RenderEngineInterop
	{
	public:
		static System::Numerics::Vector2 GetScreenSize();
		static void MoveAndResize(int x, int y, int w, int h);

		static void UpdateImGuiUI(IntPtr drawData);
		static IntPtr CreateImGuiUI(IntPtr pixels, int texWidth, int texHeight, int bytesPerPixel);
	};
}
#pragma once

namespace EduEngine
{
	private ref class RenderEngineInterop
	{
	public:
		static System::Numerics::Vector2 GetScreenSize();
		static void MoveAndResize(int x, int y, int w, int h);
	};
}
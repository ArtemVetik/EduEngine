#pragma once
#include <wtypes.h>

namespace EduEngine
{
	private value struct MouseStateWrapper
	{
		LONG    lX;
		LONG    lY;
		LONG    lZ;
		cli::array<BYTE>^ rgbButtons;
	};

	private ref class InputSystemInterop
	{
	public:
		static bool IsAnyKeyPressed();
		static bool IsKeyPressed(BYTE key);
		static MouseStateWrapper GetMouseState();
	};
}
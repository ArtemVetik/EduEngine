#pragma once
#include <wtypes.h>
#include "../../InputSystem/InputManager.h"

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
		InputSystemInterop(bool editor);

		bool IsAnyKeyPressed();
		bool IsKeyPressed(BYTE key);
		bool IsKeyDown(BYTE key);
		bool IsKeyUp(BYTE key);
		MouseStateWrapper GetMouseState();
		MouseStateWrapper GetPrevMouseState();
		System::Numerics::Vector2 GetCursorPos();

	private:
		InputManager* m_Input;
	};
}
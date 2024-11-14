#include "InputSystemInterop.h"
#include "../../InputSystem/InputManager.h"

namespace EduEngine
{
	bool InputSystemInterop::IsAnyKeyPressed()
	{
		return InputManager::GetInstance().IsAnyKeyPressed();
	}

	bool InputSystemInterop::IsKeyPressed(BYTE key)
	{
		return InputManager::GetInstance().IsKeyPressed(key);
	}

	MouseStateWrapper InputSystemInterop::GetMouseState()
	{
		auto mouseState = InputManager::GetInstance().GetMouseState();

		MouseStateWrapper managedMouseState;
		managedMouseState.lX = mouseState.lX;
		managedMouseState.lY = mouseState.lY;
		managedMouseState.lZ = mouseState.lZ;

		managedMouseState.rgbButtons = gcnew cli::array<BYTE>(8);

		for (size_t i = 0; i < 8; i++)
			managedMouseState.rgbButtons[i] = mouseState.rgbButtons[i];

		return managedMouseState;
	}
}
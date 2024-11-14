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

	bool InputSystemInterop::IsKeyDown(BYTE key)
	{
		return InputManager::GetInstance().IsKeyDown(key);
	}

	bool InputSystemInterop::IsKeyUp(BYTE key)
	{
		return InputManager::GetInstance().IsKeyUp(key);
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

	MouseStateWrapper InputSystemInterop::GetPrevMouseState()
	{
		auto mouseState = InputManager::GetInstance().GetPrevMouseState();

		MouseStateWrapper managedMouseState;
		managedMouseState.lX = mouseState.lX;
		managedMouseState.lY = mouseState.lY;
		managedMouseState.lZ = mouseState.lZ;

		managedMouseState.rgbButtons = gcnew cli::array<BYTE>(8);

		for (size_t i = 0; i < 8; i++)
			managedMouseState.rgbButtons[i] = mouseState.rgbButtons[i];

		return managedMouseState;
	}

	System::Numerics::Vector2 InputSystemInterop::GetCursorPos()
	{
		auto pos = InputManager::GetInstance().GetCursorPosition();
		return System::Numerics::Vector2(pos.x, pos.y);
	}
}
#include "InputSystemInterop.h"

namespace EduEngine
{
	InputSystemInterop::InputSystemInterop(bool editor)
	{
		if (editor)
			m_Input = &InputManager::GetEditorInstance();
		else
			m_Input = &InputManager::GetInstance();
	}
	bool InputSystemInterop::IsAnyKeyPressed()
	{
		return m_Input->IsAnyKeyPressed();
	}

	bool InputSystemInterop::IsKeyPressed(BYTE key)
	{
		return m_Input->IsKeyPressed(key);
	}

	bool InputSystemInterop::IsKeyDown(BYTE key)
	{
		return m_Input->IsKeyDown(key);
	}

	bool InputSystemInterop::IsKeyUp(BYTE key)
	{
		return m_Input->IsKeyUp(key);
	}

	bool InputSystemInterop::IsCapsLockEnabled()
	{
		return m_Input->IsCapsLockEnabled();
	}

	MouseStateWrapper InputSystemInterop::GetMouseState()
	{
		auto mouseState = m_Input->GetMouseState();

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
		auto mouseState = m_Input->GetPrevMouseState();

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
		auto pos = m_Input->GetCursorPosition();
		return System::Numerics::Vector2(pos.x, pos.y);
	}
}
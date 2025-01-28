#include "InputManager.h"
#include <iostream>

namespace EduEngine
{
	InputManager::InputManager() :
		m_directInput(nullptr),
		m_keyboardDevice(nullptr),
		m_mouseDevice(nullptr),
		m_keyboardState{},
		m_prevKeyboardState{},
		m_mouseState{},
		m_prevMouseState{},
		m_VisibleCursor(true)
	{ }

	InputManager::~InputManager()
	{
		ReleaseDevices();

		if (m_directInput)
		{
			m_directInput->Release();
			m_directInput = nullptr;
		}
	}

	InputManager& InputManager::GetInstance()
	{
		static InputManager instance;
		return instance;
	}

	InputManager& InputManager::GetEditorInstance()
	{
		static InputManager editorInstance;
		return editorInstance;
	}

	bool InputManager::Initialize(HINSTANCE hInstance, HWND hWnd)
	{
		m_window = hWnd;

		DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, nullptr);
		InitKeyboard(hWnd);
		InitMouse(hWnd);

		HRESULT result;
		result = m_keyboardDevice->Acquire();
		if (FAILED(result)) return false;

		result = m_mouseDevice->Acquire();
		if (FAILED(result)) return false;

		return true;
	}

	bool InputManager::InitKeyboard(HWND hWnd)
	{
		HRESULT result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboardDevice, nullptr);
		if (FAILED(result)) return false;

		result = m_keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(result)) return false;

		result = m_keyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(result)) return false;

		return true;
	}

	bool InputManager::InitMouse(HWND hWnd)
	{
		HRESULT result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouseDevice, nullptr);
		if (FAILED(result)) return false;

		result = m_mouseDevice->SetDataFormat(&c_dfDIMouse2);
		if (FAILED(result)) return false;

		result = m_mouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(result)) return false;

		return true;
	}

	void InputManager::Update()
	{
		HRESULT result;

		memcpy(m_prevKeyboardState, m_keyboardState, sizeof(m_keyboardState));
		result = m_keyboardDevice->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
		if (FAILED(result))
		{
			m_keyboardDevice->Acquire();
			m_keyboardDevice->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
		}

		m_prevMouseState = m_mouseState;

		result = m_mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_mouseState);
		if (FAILED(result))
		{
			m_mouseDevice->Acquire();
			m_mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_mouseState);
		}
	}

	void InputManager::SetCursorVisibility(bool visible)
	{
		if (m_VisibleCursor != visible)
		{
			m_VisibleCursor = visible;
			ShowCursor(visible);
		}
	}

	void InputManager::ClipCursorToWindow(bool clip)
	{
		if (!clip)
		{
			ClipCursor(NULL);
			return;
		}

		RECT rect;
		GetClientRect(m_window, &rect);
		MapWindowPoints(m_window, nullptr, (POINT*)&rect, 2);
		ClipCursor(&rect);
	}

	bool InputManager::IsKeyPressed(BYTE key)
	{
		return m_keyboardState[key] & 0x80;
	}

	bool InputManager::IsKeyDown(BYTE key)
	{
		return ((m_prevKeyboardState[key] & 0x80) == 0) && ((m_keyboardState[key] & 0x80) != 0);
	}

	bool InputManager::IsKeyUp(BYTE key)
	{
		return ((m_prevKeyboardState[key] & 0x80) != 0) && ((m_keyboardState[key] & 0x80) == 0);
	}

	bool InputManager::IsAnyKeyPressed()
	{
		for (int i = 0; i < 256; i++)
		{
			if (m_keyboardState[i] & 0x80)
			{
				return true;
			}
		}
		return false;
	}

	bool InputManager::IsCapsLockEnabled()
	{
		return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
	}

	POINT InputManager::GetCursorPosition()
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(m_window, &p);
		}
		return p;
	}

	DIMOUSESTATE2 InputManager::GetMouseState()
	{
		return m_mouseState;
	}

	DIMOUSESTATE2 InputManager::GetPrevMouseState()
	{
		return m_prevMouseState;
	}

	void InputManager::ReleaseDevices()
	{
		if (m_keyboardDevice) 
		{
			m_keyboardDevice->Unacquire();
			m_keyboardDevice->Release();
			m_keyboardDevice = nullptr;
		}
		if (m_mouseDevice) 
		{
			m_mouseDevice->Unacquire();
			m_mouseDevice->Release();
			m_mouseDevice = nullptr;
		}
	}
}
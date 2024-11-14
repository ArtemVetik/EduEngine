#include "InputManager.h"

namespace EduEngine
{
	InputManager::InputManager() :
		m_directInput(nullptr),
		m_keyboardDevice(nullptr),
		m_mouseDevice(nullptr),
		m_keyboardState{},
		m_mouseState{}
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

	bool InputManager::Initialize(HINSTANCE hInstance, HWND hWnd)
	{
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

		result = m_keyboardDevice->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
		if (FAILED(result))
		{
			m_keyboardDevice->Acquire();
			m_keyboardDevice->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
		}

		result = m_mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_mouseState);
		if (FAILED(result))
		{
			m_mouseDevice->Acquire();
			m_mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_mouseState);
		}
	}

	bool InputManager::IsKeyPressed(BYTE key)
	{
		return m_keyboardState[key] & 0x80;
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

	DIMOUSESTATE2 InputManager::GetMouseState()
	{
		return m_mouseState;
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
#pragma once

#include <dinput.h>
#include "InputSystemModule.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace EduEngine
{
    class INPUTSYSTEM_API InputManager
    {
    public:
        static InputManager& GetInstance();

        bool Initialize(HINSTANCE hInstance, HWND hWnd);
        void Update();
        bool IsKeyPressed(BYTE key);
        bool IsAnyKeyPressed();
        POINT GetCursorPosition();
        DIMOUSESTATE2 GetMouseState();
        DIMOUSESTATE2 GetPrevMouseState();

    private:
        InputManager();
        ~InputManager();

        bool InitKeyboard(HWND hWnd);
        bool InitMouse(HWND hWnd);

        void ReleaseDevices();

    private:
        IDirectInput8* m_directInput;
        IDirectInputDevice8* m_keyboardDevice;
        IDirectInputDevice8* m_mouseDevice;

        BYTE m_keyboardState[256];
        DIMOUSESTATE2 m_mouseState;
        DIMOUSESTATE2 m_prevMouseState;
    };
}
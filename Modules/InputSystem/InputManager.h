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
        static InputManager& GetEditorInstance();

        bool Initialize(HINSTANCE hInstance, HWND hWnd);
        void Update();
        void SetCursorVisibility(bool visible);
        void ClipCursorToWindow(bool clip);
        bool IsKeyPressed(BYTE key);
        bool IsKeyDown(BYTE key);
        bool IsKeyUp(BYTE key);
        bool IsAnyKeyPressed();
        bool IsCapsLockEnabled();
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
        BYTE m_prevKeyboardState[256];
        DIMOUSESTATE2 m_mouseState;
        DIMOUSESTATE2 m_prevMouseState;

        HWND m_window;
        bool m_VisibleCursor;
    };
}
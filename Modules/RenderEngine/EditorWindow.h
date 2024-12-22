#pragma once
#include "WindowBase.h"

namespace EduEngine
{
    class RENDERENGINE_API EditorWindow : public WindowBase
    {
    public:
        EditorWindow(HINSTANCE hInstance, int width, int height);

        HWND _CreateWindow(HINSTANCE hInstance) override;
        void OnResize(UINT w, UINT h) override;

        HWND GetHostWindow() const { return m_HostWindow; }

        static EditorWindow* GetInstance() { return m_Instance; }

    private:
        static EditorWindow* m_Instance;

        HWND m_HostWindow;
    };
}
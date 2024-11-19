#include "pch.h"
#include "RuntimeWindow.h"
#include "RenderEngine.h"

namespace EduEngine
{
	LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return RuntimeWindow::GetInstance()->MsgProc(hwnd, msg, wParam, lParam);
	}

    RuntimeWindow* RuntimeWindow::m_Instance = nullptr;

    RuntimeWindow::RuntimeWindow(HINSTANCE hInstance, int width, int height) :
        WindowBase(hInstance, width, height)
    {
        assert(m_Instance == nullptr);
        m_Instance = this;
    }

    HWND RuntimeWindow::_CreateWindow(HINSTANCE hInstance)
    {
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"RuntimeWindow";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		RECT R = { 0, 0, GetClientWidth(), GetClientHeight() };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		auto mainWindowHandle = CreateWindow(wc.lpszClassName, L"Runtime_Window",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInstance, 0);
		if (!mainWindowHandle)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(mainWindowHandle, SW_SHOW);
		UpdateWindow(mainWindowHandle);

		return mainWindowHandle;
    }

    void RuntimeWindow::OnResize(UINT w, UINT h)
    {
		if (RenderEngine::GetInstance())
			RenderEngine::GetInstance()->Resize(w, h);
    }
}
#include "pch.h"
#include "EditorWindow.h"
#include "EditorRenderEngine.h"

namespace EduEngine
{
	LRESULT CALLBACK HostWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return EditorWindow::GetInstance()->MsgProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK EditorWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	EditorWindow* EditorWindow::m_Instance = nullptr;

	EditorWindow::EditorWindow(HINSTANCE hInstance, int width, int height) :
		WindowBase(hInstance, width, height)
	{
		assert(m_Instance == nullptr);
		m_Instance = this;
	}

	HWND EditorWindow::_CreateWindow(HINSTANCE hInstance)
	{
		WNDCLASS wc = { 0 };
		wc.lpfnWndProc = HostWindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
		wc.lpszClassName = L"HostWindow";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		auto hostWindowHandle = CreateWindowEx(
			0,
			L"HostWindow",
			L"Host Window",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			GetClientWidth(),
			GetClientHeight(),
			0,
			NULL,
			GetModuleHandle(NULL),
			NULL
		);

		if (!hostWindowHandle)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(hostWindowHandle, SW_SHOWNORMAL);
		UpdateWindow(hostWindowHandle);

		wc = { 0 };
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = EditorWindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszClassName = L"EditorWindow";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		/*RECT R = { 0, 0, m_ScreenWidth, m_ScreenHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;*/

		auto windowHandle = CreateWindowEx(
			WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST,
			L"EditorWindow",
			L"Editor Window",
			WS_CHILD | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			GetClientWidth(),
			GetClientHeight(),
			hostWindowHandle,
			0,
			GetModuleHandle(NULL),
			0
		);

		if (!windowHandle)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(windowHandle, SW_SHOW);
		UpdateWindow(windowHandle);

		return windowHandle;
	}

	void EditorWindow::OnResize(UINT w, UINT h)
	{
		SetWindowPos(GetMainWindow(), 0, 0, 0, w, h, SWP_NOZORDER);

		if (EditorRenderEngine::GetInstance())
			EditorRenderEngine::GetInstance()->Resize(w, h);
	}
}
#include "pch.h"
#include "RuntimeWindow.h"
#include "RenderEngine.h"

namespace EduEngine
{
	LRESULT CALLBACK RuntimeWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return RuntimeWindow::GetInstance()->MsgProc(hwnd, msg, wParam, lParam);
	}

	RuntimeWindow* RuntimeWindow::m_Instance = nullptr;

	RuntimeWindow::RuntimeWindow(HINSTANCE hInstance, int width, int height, HWND parent /* = 0*/) :
		WindowBase(hInstance, width, height)
	{
		assert(m_Instance == nullptr);
		m_Instance = this;

		m_Parent = parent;
	}

	void RuntimeWindow::GetPosition(UINT& x, UINT& y, UINT& w, UINT& h)
	{
		HWND hwndParent = GetParent(GetMainWindow());

		RECT rect;
		GetWindowRect(GetMainWindow(), &rect);
		x = rect.left;
		y = rect.top;

		if (hwndParent)
		{
			POINT pt = { x, y };
			ScreenToClient(hwndParent, &pt);
			x = pt.x;
			y = pt.y;
		}

		w = rect.right - rect.left;
		h = rect.bottom - rect.top;
	}

	void RuntimeWindow::SetPosition(UINT x, UINT y, UINT w, UINT h)
	{
		SetWindowPos(GetMainWindow(), NULL, x, y, w, h, SWP_NOZORDER);
	}

	HWND RuntimeWindow::_CreateWindow(HINSTANCE hInstance)
	{
		WNDCLASS wc = { 0 };
		wc.lpfnWndProc = RuntimeWindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = L"RuntimeWindow";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		auto windowHandle = CreateWindowEx(
			0,
			L"RuntimeWindow",
			L"Runtime Window",
			(m_Parent ? (WS_CHILD | WS_VISIBLE) : (WS_OVERLAPPEDWINDOW)),
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			GetClientWidth(),
			GetClientHeight(),
			m_Parent,
			NULL,
			GetModuleHandle(NULL),
			NULL
		);

		if (!windowHandle)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(windowHandle, SW_SHOWNORMAL);
		UpdateWindow(windowHandle);

		return windowHandle;
	}

	void RuntimeWindow::OnResize(UINT w, UINT h)
	{
		if (RenderEngine::GetInstance())
			RenderEngine::GetInstance()->Resize(w, h);
	}
}
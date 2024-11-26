#include "pch.h"
#include "WindowBase.h"

namespace EduEngine
{
	WindowBase::WindowBase(HINSTANCE hInstance, int width, int height) :
		m_ApplicationInstanceHandle(hInstance),
		m_ScreenWidth(width),
		m_ScreenHeight(height)
	{
	}

	bool WindowBase::Initialize()
	{
		m_MainWindowHandle = _CreateWindow(m_ApplicationInstanceHandle);
		return true;
	}

	void WindowBase::AddFocusCallback(FocusCallback callback)
	{
		m_FocusCallbacks.push_back(callback);
	}

	void WindowBase::RemoveFocusCallback(FocusCallback callbackToRemove)
	{
		m_FocusCallbacks.erase(std::remove_if(m_FocusCallbacks.begin(), m_FocusCallbacks.end(),
			[&callbackToRemove](const FocusCallback& callback)
			{
				return callback.target_type() == callbackToRemove.target_type() &&
					callback.target<void(bool)>() == callbackToRemove.target<void(bool)>();
			}),
			m_FocusCallbacks.end());
	}

	LRESULT WindowBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_KILLFOCUS:
			for (auto& callback : m_FocusCallbacks)
				callback(false);
			break;
		case WM_SETFOCUS:
			for (auto& callback : m_FocusCallbacks)
				callback(true);
			break;
		case WM_SIZE:
			m_ScreenWidth = LOWORD(lParam);
			m_ScreenHeight = HIWORD(lParam);

			if (wParam == SIZE_MINIMIZED)
			{
				m_ApplicationPaused = true;
				m_ApplicationMinimized = true;
				m_ApplicationMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_ApplicationPaused = false;
				m_ApplicationMinimized = false;
				m_ApplicationMaximized = true;
				OnResize(m_ScreenWidth, m_ScreenHeight);
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (m_ApplicationMinimized)
				{
					m_ApplicationPaused = false;
					m_ApplicationMinimized = false;
					OnResize(m_ScreenWidth, m_ScreenHeight);
				}
				// Restoring from maximized state?
				else if (m_ApplicationMaximized)
				{
					m_ApplicationPaused = false;
					m_ApplicationMaximized = false;
					OnResize(m_ScreenWidth, m_ScreenHeight);
				}
				else if (m_ApplicationResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize(m_ScreenWidth, m_ScreenHeight);
				}
			}
			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

			/*case WM_GETMINMAXINFO:
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
				((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
				return 0;*/
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
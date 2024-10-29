#include "pch.h"
#include "Window.h"
#include <cassert>

namespace EduEngine
{
	static Window* mInstance;

	LRESULT CALLBACK
		MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return Window::GetInstance()->MsgProc(hwnd, msg, wParam, lParam);
	}

	Window::Window(HINSTANCE hInstance) : mApplicationInstanceHandle(hInstance)
	{
		assert(mInstance == nullptr);
		mInstance = this;
	}

	Window* Window::GetInstance()
	{
		return mInstance;
	}

	bool Window::Initialize()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = mApplicationInstanceHandle;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWindow";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		RECT R = { 0, 0, mScreenWidth, mScreenHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		mMainWindowHandle = CreateWindow(L"MainWindow", mMainWindowCaption,
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mApplicationInstanceHandle, 0);
		if (!mMainWindowHandle)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(mMainWindowHandle, SW_SHOW);
		UpdateWindow(mMainWindowHandle);

		return true;
	}

	int Window::GetClientWidth() const
	{
		return mScreenWidth;
	}

	int Window::GetClientHeight() const
	{
		return mScreenHeight;
	}

	int Window::IsPaused() const
	{
		return mApplicationPaused;
	}

	HWND Window::GetMainWindow() const
	{
		return mMainWindowHandle;
	}

	float Window::AspectRatio() const
	{
		return static_cast<float>(mScreenWidth) / mScreenHeight;
	}

	LRESULT Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

		switch (msg)
		{
		case WM_SIZE:
			mScreenWidth = LOWORD(lParam);
			mScreenHeight = HIWORD(lParam);

			if (wParam == SIZE_MINIMIZED)
			{
				mApplicationPaused = true;
				mApplicationMinimized = true;
				mApplicationMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mApplicationPaused = false;
				mApplicationMinimized = false;
				mApplicationMaximized = true;
				if (mResizeEvent) mResizeEvent();
			}
			else if (wParam == SIZE_RESTORED)
			{
				// Restoring from minimized state?
				if (mApplicationMinimized)
				{
					mApplicationPaused = false;
					mApplicationMinimized = false;
					if (mResizeEvent) mResizeEvent();
				}
				// Restoring from maximized state?
				else if (mApplicationMaximized)
				{
					mApplicationPaused = false;
					mApplicationMaximized = false;
					if (mResizeEvent) mResizeEvent();
				}
				else if (mApplicationResizing)
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
					if (mResizeEvent) mResizeEvent();
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

	void Window::SetResizeEvent(const std::function<void()>& resizeEvent)
	{
		mResizeEvent = resizeEvent;
	}
}
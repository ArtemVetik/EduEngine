#pragma once
#include "framework.h"
#include <vector>
#include <functional>

namespace EduEngine
{
	class RENDERENGINE_API WindowBase
	{
	public:
		WindowBase(HINSTANCE hInstance, int width, int height);
		bool Initialize();

		int GetClientWidth() const { return m_ScreenWidth; }
		int GetClientHeight() const { return m_ScreenHeight; }
		int IsPaused() const { return m_ApplicationPaused; }
		HWND GetMainWindow() const { return m_MainWindowHandle; }
		float AspectRatio() const { return static_cast<float>(m_ScreenWidth) / m_ScreenHeight; }

		using FocusCallback = std::function<void(bool)>;

		void AddFocusCallback(FocusCallback callback);
		void RemoveFocusCallback(FocusCallback callback);

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	protected:
		virtual HWND _CreateWindow(HINSTANCE hInstance) = 0;
		virtual void OnResize(UINT w, UINT h) = 0;

	private:
		HINSTANCE m_ApplicationInstanceHandle = nullptr;
		HWND m_MainWindowHandle = nullptr;

		int m_ScreenWidth = 1280;
		int m_ScreenHeight = 720;
		bool m_ApplicationPaused = false;
		bool m_ApplicationMinimized = false;
		bool m_ApplicationMaximized = false;
		bool m_ApplicationResizing = false;
		bool m_ApplicationFullScreen = false;


		std::vector<FocusCallback> m_FocusCallbacks;
	};
}
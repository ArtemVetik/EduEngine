#pragma once
#include "pch.h"

namespace EduEngine
{
	class GRAPHICS_API Window
	{
	public:
		Window(HINSTANCE hInstance);
		bool Initialize();

		int GetClientWidth() const;
		int GetClientHeight() const;
		int IsPaused() const;
		HWND GetMainWindow() const;
		float AspectRatio() const;

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static Window* GetInstance();

	private:
		static Window* m_Instance;

		HINSTANCE mApplicationInstanceHandle = nullptr;
		HWND mMainWindowHandle = nullptr;

		int m_ScreenWidth = 1280;
		int m_ScreenHeight = 720;
		bool mApplicationPaused = false;
		bool mApplicationMinimized = false;
		bool mApplicationMaximized = false;
		bool mApplicationResizing = false;
		bool mApplicationFullScreen = false;

		const wchar_t* mMainWindowCaption = L"d3d12 App";
	};
}
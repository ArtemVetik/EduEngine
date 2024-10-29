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

		void SetResizeEvent(const std::function<void()>& resizeEvent);

		static Window* GetInstance();
	private:
		std::function<void()> mResizeEvent;

		HINSTANCE mApplicationInstanceHandle = nullptr;
		HWND mMainWindowHandle = nullptr;

		int mScreenWidth = 1280;
		int mScreenHeight = 720;
		bool mApplicationPaused = false;
		bool mApplicationMinimized = false;
		bool mApplicationMaximized = false;
		bool mApplicationResizing = false;
		bool mApplicationFullScreen = false;

		const wchar_t* mMainWindowCaption = L"d3d12 App";
	};
}
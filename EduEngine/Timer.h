#pragma once
#include <wtypes.h>

namespace EduEngine
{
	class Timer
	{
	public:
		Timer(HWND handle, LPCTSTR windowTitle);
		~Timer();

		float GetDeltaTime() const;
		float GetTotalTime() const;

		void UpdateTimer();
		void UpdateTitleBarStats();

	private:
		double perfCounterSeconds;

		float totalTime;
		float deltaTime;

		__int64 startTime;
		__int64 currentTime;
		__int64 previousTime;

		int fpsFrameCount;
		float fpsTimeElapsed;

		HWND hwnd;
		LPCTSTR windowTitle;
	};
}
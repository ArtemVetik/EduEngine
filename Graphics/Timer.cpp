#include "pch.h"
#include "Timer.h"
#include <sstream>

namespace EduEngine
{
	Timer::Timer()
	{

	}

	Timer::Timer(HWND handle, LPCTSTR windowTitle) : hwnd(handle), windowTitle(windowTitle)
	{
		fpsFrameCount = 0;
		fpsTimeElapsed = 0;

		__int64 prefFreq;
		QueryPerformanceFrequency((LARGE_INTEGER*)&prefFreq);
		perfCounterSeconds = 1.0 / (double)prefFreq;

		__int64 now;
		QueryPerformanceCounter((LARGE_INTEGER*)&now);
		startTime = now;
		currentTime = now;
		previousTime = now;
	}

	Timer::~Timer()
	{
	}

	float Timer::GetDeltaTime() const
	{
		return deltaTime;
	}

	float Timer::GetTotalTime() const
	{
		return totalTime;
	}

	void Timer::UpdateTimer()
	{
		__int64 now;
		QueryPerformanceCounter((LARGE_INTEGER*)&now);
		currentTime = now;

		deltaTime = max((float)((currentTime - previousTime) * perfCounterSeconds), 0.0f);

		totalTime = (float)((currentTime - startTime) * perfCounterSeconds);

		previousTime = currentTime;
	}

	void Timer::UpdateTitleBarStats()
	{
		fpsFrameCount++;

		float timeDiff = 0.0f;
		timeDiff = totalTime - fpsTimeElapsed;
		if (timeDiff < 1.0f)
			return;

		// How long did each frame take?  (Approx)
		float mspf = 1000.0f / (float)fpsFrameCount;

		std::wstringstream out;
		out.precision(6);

		out << windowTitle <<
			"    FPS : " << fpsFrameCount <<
			"    Frame time: " << mspf << " ms" << "\0";

		fpsFrameCount = 0;
		fpsTimeElapsed += 1.0f;

		SetWindowText(hwnd, out.str().c_str());
	}
}
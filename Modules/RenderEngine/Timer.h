#pragma once
#include "framework.h"
#include <wtypes.h>

namespace EduEngine
{
	class RENDERENGINE_API Timer
	{
	public:
		Timer(HWND handle, LPCTSTR windowTitle);
		~Timer();

		float GetDeltaTime() const;
		float GetTotalTime() const;

		void UpdateTimer();
		bool UpdateTitleBarStats(int& fps, float& mspf);

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
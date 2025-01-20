#pragma once
#include <future>
#include <sstream>
#include <shobjidl.h>

namespace EduEngine
{
	class Common
	{
	public:
		static std::wstring OpenFolderDialog(bool folder = true);

		static void UpdateWindowTitle(HWND window, int rFps, float rMspf, int* eFps, float* eMspf);

		static void AllocWinConsole();

		template<typename T>
		static bool future_is_ready(std::future<T>& t) {
			return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		}
	};
}
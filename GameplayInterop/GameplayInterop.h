#pragma once

#ifdef GAMEPLAYINTEROP_EXPORTS
#define GAMEPLAYINTEROP_API __declspec(dllexport)
#else
#define GAMEPLAYINTEROP_API __declspec(dllimport)
#endif

namespace EduEngine
{
	class GAMEPLAYINTEROP_API GameplayInterop
	{
	public:
		static void Initialize();
		static void Update();
	};
}
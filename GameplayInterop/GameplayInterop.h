#pragma once

#ifdef GAMEPLAYINTEROP_EXPORTS
#define GAMEPLAYINTEROP_API __declspec(dllexport)
#else
#define GAMEPLAYINTEROP_API __declspec(dllimport)
#endif

#include <string>

namespace EduEngine
{
	class GAMEPLAYINTEROP_API GameplayInterop
	{
	public:
		static void Initialize();
		static void Update();
		static void Render();
	};

	class GAMEPLAYINTEROP_API EditorInterop
	{
	public:
		static void Initialize(std::wstring rootPath);
		static void Update();
		static void Destroy();
	};
}
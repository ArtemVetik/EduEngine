#pragma once

#ifdef GAMEPLAYINTEROP_EXPORTS
#define GAMEPLAYINTEROP_API __declspec(dllexport)
#else
#define GAMEPLAYINTEROP_API __declspec(dllimport)
#endif

#include <string>

namespace EduEngine
{
	enum class EngineState
	{
		Editor,
		Runtime
	};

	class GAMEPLAYINTEROP_API GameplayInterop
	{
	public:
		static void Update();
		static void Render();
		static void Destroy();
	};

	class GAMEPLAYINTEROP_API EditorInterop
	{
	public:
		static void Initialize(std::wstring assetsPath, std::wstring dllPath, bool withEditor);
		static void Update();
		static void RenderScene();
		static void Destroy();
		static EngineState GetEngineState();
		static bool InspectScene();
	};
}
#pragma once
#include <string>

using namespace System;

namespace EduEngine
{
	public ref class InternalGameplayInterop
	{
	public:
		static void Update()
		{
			GameplayFramework::Update();
		}

		static void Render()
		{
			GameplayFramework::Render();
		}
	};

	public ref class InternalEditorInterop
	{
	public:
		static void Initialize(std::wstring assetsPath, std::wstring dllPath)
		{
			String^ cAssetPath = gcnew String(assetsPath.c_str());
			String^ cDllPath = gcnew String(dllPath.c_str());
			Editor::EduEditor::Initialize(cAssetPath, cDllPath);
		}

		static void Update()
		{
			Editor::EduEditor::Update();
		}

		static void RenderScene()
		{
			Editor::EduEditor::RenderScene();
		}

		static void Destroy()
		{
			Editor::EduEditor::Destroy();
		}

		static EngineState GetEngineState()
		{
			return (EngineState)Editor::EngineStateManager::CurrentState;
		}
	};
}

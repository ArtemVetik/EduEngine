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

		static void PhysicsUpdate()
		{
			GameplayFramework::PhysicsUpdate();
		}

		static void Render()
		{
			GameplayFramework::Render();
		}

		static void Destroy()
		{
			GameplayFramework::Destroy();
		}
	};

	public ref class InternalEditorInterop
	{
	public:
		static void Initialize(std::wstring assetsPath, std::wstring dllPath, bool withEditor)
		{
			String^ cAssetPath = gcnew String(assetsPath.c_str());
			String^ cDllPath = gcnew String(dllPath.c_str());
			Editor::EduEditor::Initialize(cAssetPath, cDllPath, withEditor);
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

		static bool GetInspectScene()
		{
			return Editor::EngineStateManager::InspectScene;
		}
	};
}

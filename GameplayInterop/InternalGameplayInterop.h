#pragma once
#include <string>

using namespace System;

namespace EduEngine
{
	public ref class InternalGameplayInterop
	{
	public:
		static void Initialize()
		{
			Scripts::TestMain::Main();
		}

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
		static void Initialize(std::wstring rootPath)
		{
			String^ s = gcnew String(rootPath.c_str());
			Editor::EduEditor::Initialize(s);
		}

		static void Update()
		{
			Editor::EduEditor::Update();
		}

		static void Destroy()
		{
			Editor::EduEditor::Destroy();
		}
	};
}

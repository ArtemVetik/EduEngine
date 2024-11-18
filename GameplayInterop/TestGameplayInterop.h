#pragma once

using namespace System;

namespace EduEngine
{
	public ref class TestGameplayInterop
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
	};

	public ref class TestEditorInterop
	{
	public:
		static void Initialize()
		{
			Editor::EduEditor::Initialize();
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

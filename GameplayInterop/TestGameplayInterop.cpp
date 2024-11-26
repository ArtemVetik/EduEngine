#include "GameplayInterop.h"
#include "TestGameplayInterop.h"

namespace EduEngine
{
	void GameplayInterop::Initialize()
	{
		TestGameplayInterop::Initialize();
	}

	void GameplayInterop::Update()
	{
		TestGameplayInterop::Update();
	}

	void EditorInterop::Initialize(std::wstring rootPath)
	{
		TestEditorInterop::Initialize(rootPath);
	}

	void EditorInterop::Update()
	{
		TestEditorInterop::Update();
	}

	void EditorInterop::Destroy()
	{
		TestEditorInterop::Destroy();
	}
}
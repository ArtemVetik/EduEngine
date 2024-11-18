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

	void EditorInterop::Initialize()
	{
		TestEditorInterop::Initialize();
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
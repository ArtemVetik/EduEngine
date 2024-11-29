#include "GameplayInterop.h"
#include "InternalGameplayInterop.h"

namespace EduEngine
{
	void GameplayInterop::Update()
	{
		InternalGameplayInterop::Update();
	}

	void GameplayInterop::Render()
	{
		InternalGameplayInterop::Render();
	}

	void EditorInterop::Initialize(std::wstring rootPath)
	{
		InternalEditorInterop::Initialize(rootPath);
	}

	void EditorInterop::Update()
	{
		InternalEditorInterop::Update();
	}

	void EditorInterop::RenderScene()
	{
		InternalEditorInterop::RenderScene();
	}

	void EditorInterop::Destroy()
	{
		InternalEditorInterop::Destroy();
	}

	EngineState EditorInterop::GetEngineState()
	{
		return InternalEditorInterop::GetEngineState();
	}
}
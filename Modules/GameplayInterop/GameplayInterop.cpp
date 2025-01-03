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

	void GameplayInterop::Destroy()
	{
		InternalGameplayInterop::Destroy();
	}

	void EditorInterop::Initialize(std::wstring assetsPath, std::wstring dllPath, bool withEditor)
	{
		InternalEditorInterop::Initialize(assetsPath, dllPath, withEditor);
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

	bool EditorInterop::InspectScene()
	{
		return InternalEditorInterop::GetInspectScene();
	}
}
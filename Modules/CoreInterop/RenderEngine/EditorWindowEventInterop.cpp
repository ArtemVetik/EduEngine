#include "EditorWindowEventInterop.h"
#include "../../RenderEngine/EditorWindow.h"

namespace EduEngine
{
	static void GlobalOnFocusChanged(bool focus)
	{
		EditorWindowEventInterop::OnFocusChanged(focus);
	}

	void EditorWindowEventInterop::AddFocusCallback(System::Action<bool>^ callback)
	{
		if (m_Subscribers == 0)
			EditorWindow::GetInstance()->AddFocusCallback(GlobalOnFocusChanged);

		FocusChanged += callback;
		m_Subscribers++;
	}

	void EditorWindowEventInterop::RemoveFocusCallback(System::Action<bool>^ callback)
	{
		if (m_Subscribers == 0)
			return;

		FocusChanged -= callback;
		m_Subscribers--;
		
		if (m_Subscribers == 0)
			EditorWindow::GetInstance()->RemoveFocusCallback(GlobalOnFocusChanged);
	}

	void EditorWindowEventInterop::OnFocusChanged(bool value)
	{
		FocusChanged(value);
	}
}
#pragma once

namespace EduEngine
{
	private ref class EditorWindowEventInterop
	{
	public:
		static void AddFocusCallback(System::Action<bool>^ callback);
		static void RemoveFocusCallback(System::Action<bool>^ callback);

		static void OnFocusChanged(bool value);

    private:
		static event System::Action<bool>^ FocusChanged;

		static int m_Subscribers = 0;
	};
}
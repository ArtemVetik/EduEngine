#pragma once
#include "framework.h"
#include "WindowBase.h"

namespace EduEngine
{
	class RENDERENGINE_API RuntimeWindow : public WindowBase
	{
	public:
		RuntimeWindow(HINSTANCE hInstance, int width, int height);

		static RuntimeWindow* GetInstance() { return m_Instance; }

	protected:
		HWND _CreateWindow(HINSTANCE hInstance) override;
		void OnResize(UINT w, UINT h) override;

	private:
		static RuntimeWindow* m_Instance;
	};
}
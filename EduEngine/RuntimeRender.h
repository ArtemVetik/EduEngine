#pragma once
#include "../RenderEngine/IRenderEngine.h"
#include "../GameplayInterop/GameplayInterop.h"

namespace EduEngine
{
	class RuntimeRender
	{
	public:
		RuntimeRender(IRenderEngine* renderEngine) :
			m_RenderEngine(renderEngine)
		{ }

		void RenderRuntime()
		{
			m_RenderEngine->BeginDraw();
			GameplayInterop::Render();
			m_RenderEngine->EndDraw();
		}

		void RenderEditor()
		{
			m_RenderEngine->BeginDraw();
			EditorInterop::RenderScene();
			m_RenderEngine->EndDraw();
		}

	private:
		IRenderEngine* m_RenderEngine;
	};
}
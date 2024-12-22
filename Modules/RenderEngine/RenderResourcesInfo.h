#pragma once
#include <sstream>

#include "IRenderEngine.h"
#include "RenderEngine.h"
#include "EditorRenderEngine.h"

namespace EduEngine
{
	class RenderResourcesInfo : public IRenderResourcesInfo
	{
	public:
		RenderResourcesInfo(IRenderEngine* renderEngine, IEditorRenderEngine* editorRenderEngine);

		const char* GetObjectsInfo(bool onlyUsed = false) override;

	private:
		RenderEngine* m_RenderEngine;
		EditorRenderEngine* m_EditorRenderEngine;

		std::stringstream m_Oss;
		std::string m_Buffer;
	};
}
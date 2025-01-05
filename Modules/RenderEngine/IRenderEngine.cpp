#include "pch.h"
#include "IRenderEngine.h"
#include "RenderEngine.h"
#include "EditorRenderEngine.h"
#include "RenderResourcesInfo.h"

namespace EduEngine
{
    std::shared_ptr<IRenderEngine> IRenderEngine::Create(const RuntimeWindow& mainWindow, const Timer& timer)
    {
        auto renderEngine = std::make_shared<RenderEngine>(timer);
        renderEngine->StartUp(mainWindow);

        return renderEngine;
    }

    std::shared_ptr<IEditorRenderEngine> IEditorRenderEngine::Create(const EditorWindow& mainWindow)
    {
        auto editorRenderEngine = std::make_shared<EditorRenderEngine>();
        editorRenderEngine->StartUp(mainWindow);

        return editorRenderEngine;
    }

    std::shared_ptr<IRenderResourcesInfo> IRenderResourcesInfo::Create(IRenderEngine* renderEngine, IEditorRenderEngine* editorRenderEngine)
    {
        return std::make_shared<RenderResourcesInfo>(renderEngine, editorRenderEngine);
    }
}
#include "pch.h"
#include "IRenderEngine.h"
#include "RenderEngine.h"
#include "EditorRenderEngine.h"

namespace EduEngine
{
    std::shared_ptr<IRenderEngine> IRenderEngine::Create(const RuntimeWindow& mainWindow)
    {
        auto renderEngine = std::make_shared<RenderEngine>();
        renderEngine->StartUp(mainWindow);

        return renderEngine;
    }

    std::shared_ptr<IEditorRenderEngine> IEditorRenderEngine::Create(const EditorWindow& mainWindow)
    {
        auto editorRenderEngine = std::make_shared<EditorRenderEngine>();
        editorRenderEngine->StartUp(mainWindow);

        return editorRenderEngine;
    }
}
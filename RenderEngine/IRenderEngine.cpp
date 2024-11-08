#include "pch.h"
#include "IRenderEngine.h"
#include "RenderEngine.h"

namespace EduEngine
{
    std::shared_ptr<IRenderEngine> IRenderEngine::Create(const Window& mainWindow)
    {
        auto renderEngine = std::make_shared<RenderEngine>();
        renderEngine->StartUp(mainWindow);

        return renderEngine;
    }
}
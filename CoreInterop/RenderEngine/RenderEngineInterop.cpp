#include "RenderEngineInterop.h"
#include "../CoreSystems.h"

namespace EduEngine
{
    System::Numerics::Vector2 RenderEngineInterop::GetScreenSize()
    {
        auto size = CoreSystems::GetInstance()->GetRenderEngine()->GetScreenSize();
        return System::Numerics::Vector2(size.x, size.y);
    }

    void* RenderEngineInterop::CreateImGuiEditor(void* pixels, int texWidth, int texHeight, int bytesPerPixel)
    {
        return CoreSystems::GetInstance()->GetRenderEngine()->CreateEditorImGuiUI(pixels, texWidth, texHeight, bytesPerPixel);
    }

    void RenderEngineInterop::UpdateImGui(void* drawData)
    {
        CoreSystems::GetInstance()->GetRenderEngine()->UpdateEditor(static_cast<ImDrawData*>(drawData));
    }
}
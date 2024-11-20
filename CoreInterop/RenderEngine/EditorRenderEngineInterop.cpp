#include "EditorRenderEngineInterop.h"
#include "../CoreSystems.h"

namespace EduEngine
{
    System::Numerics::Vector2 EditorRenderEngineInterop::GetEditorSize()
    {
        auto size =  CoreSystems::GetInstance()->GetEditorRenderEngine()->GetScreenSize();
        return System::Numerics::Vector2(size.x, size.y);
    }

    void* EditorRenderEngineInterop::CreateImGuiEditor(void* pixels, int texWidth, int texHeight, int bytesPerPixel)
    {
        return CoreSystems::GetInstance()->GetEditorRenderEngine()->CreateEditorImGuiUI(pixels, texWidth, texHeight, bytesPerPixel);
    }

    void EditorRenderEngineInterop::UpdateImGui(void* drawData)
    {
        CoreSystems::GetInstance()->GetEditorRenderEngine()->UpdateEditor(static_cast<ImDrawData*>(drawData));
    }
}
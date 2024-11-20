#include "RenderEngineInterop.h"
#include "../CoreSystems.h"

namespace EduEngine
{
    System::Numerics::Vector2 RenderEngineInterop::GetScreenSize()
    {
        auto size = CoreSystems::GetInstance()->GetRenderEngine()->GetScreenSize();
        return System::Numerics::Vector2(size.x, size.y);
    }

    void RenderEngineInterop::MoveAndResize(int x, int y, int w, int h)
    {
        CoreSystems::GetInstance()->GetRenderEngine()->MoveAndResize(x, y, w, h);
    }
}

using System.Numerics;

namespace EduEngine
{
    public static class Screen
    {
        public static Vector2 Size => RenderEngineInterop.GetScreenSize();
    }
}

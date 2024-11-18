using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    public class EduEditor
    {
        private static ImGuiInput _input = new ImGuiInput();

        public static unsafe void Initialize()
        {
            ImGui.CreateContext();
            var io = ImGui.GetIO();
            io.BackendFlags |= ImGuiBackendFlags.RendererHasVtxOffset;
            io.ConfigFlags |= ImGuiConfigFlags.NavEnableKeyboard | ImGuiConfigFlags.DockingEnable;
            io.Fonts.Flags |= ImFontAtlasFlags.NoBakedLines;
            io.DisplaySize = Screen.Size;
            io.DisplayFramebufferScale = Vector2.One;
            io.DeltaTime = 1f / 60f;

            io.Fonts.GetTexDataAsRGBA32(out IntPtr pixels, out int width, out int height, out int bytesPerPixel);
            void* texId = RenderEngineInterop.CreateImGuiEditor((void*)pixels, width, height, bytesPerPixel);
            io.Fonts.SetTexID((IntPtr)texId);
            io.Fonts.ClearTexData();
        }

        public static unsafe void Update()
        {
            _input.ProcessInput();

            ImGui.NewFrame();
            ImGui.ShowDemoWindow();
            ImGui.Render();

            RenderEngineInterop.UpdateImGui(ImGui.GetDrawData().NativePtr);
        }

        public static void Destroy()
        {
            ImGui.DestroyContext(ImGui.GetCurrentContext());
        }
    }
}

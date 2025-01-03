using ImGuiNET;
using System.Numerics;

namespace EduEngine
{
    public class OverlayUI
    {
        private static OverlayUI _instance = null;

        private ImGuiInput _input;
        private nint? _prevContextId = null;

        internal nint ContextId { get; private set; }

        private OverlayUI()
        {
            nint prevContextId = ImGui.GetCurrentContext();

            ContextId = ImGui.CreateContext();
            ImGui.SetCurrentContext(ContextId);
            var io = ImGui.GetIO();
            io.BackendFlags |= ImGuiBackendFlags.RendererHasVtxOffset;
            io.ConfigFlags |= ImGuiConfigFlags.NavEnableKeyboard | ImGuiConfigFlags.DockingEnable;
            io.Fonts.Flags |= ImFontAtlasFlags.NoBakedLines;
            io.DisplaySize = RenderEngineInterop.GetScreenSize();
            io.DisplayFramebufferScale = Vector2.One;
            io.DeltaTime = 1f / 60f;

            io.Fonts.GetTexDataAsRGBA32(out IntPtr pixels, out int width, out int height, out int bytesPerPixel);
            IntPtr texId = RenderEngineInterop.CreateImGuiUI(pixels, width, height, bytesPerPixel);
            io.Fonts.SetTexID(texId);
            io.Fonts.ClearTexData();

            ImGui.SetCurrentContext(prevContextId);

            _input = new ImGuiInput();
        }

        public static OverlayUI GetInstance() => _instance;

        public static void Initialize()
        {
            _instance ??= new OverlayUI();
        }

        public static void Destroy()
        {
            if (_instance != null)
            {
                ImGui.DestroyContext(_instance.ContextId);
                _instance = null;
            }
        }

        public void BeginContext()
        {
            if (_prevContextId != null)
                throw new InvalidOperationException();

            _prevContextId = ImGui.GetCurrentContext();
            ImGui.SetCurrentContext(ContextId);

            _input.ProcessInput(Input.Runtime, RenderEngineInterop.GetScreenSize(), EduTime.DeltaTime, EduTime.TotalTime);
            ImGui.NewFrame();
        }

        public void EndContext()
        {
            if (_prevContextId == null)
                throw new InvalidOperationException();

            ImGui.SetCurrentContext(_prevContextId.Value);
            _prevContextId = null;
        }
    }
}

using ImGuiNET;
using ImGuizmoNET;
using System.Numerics;

namespace EduEngine.Editor
{
    public class EduEditor
    {
        private static ImGuiInput _input = new ImGuiInput();

        public static unsafe void Initialize(string rootPath)
        {
            ImGui.CreateContext();
            var io = ImGui.GetIO();
            io.BackendFlags |= ImGuiBackendFlags.RendererHasVtxOffset;
            io.ConfigFlags |= ImGuiConfigFlags.NavEnableKeyboard | ImGuiConfigFlags.DockingEnable;
            io.Fonts.Flags |= ImFontAtlasFlags.NoBakedLines;
            io.DisplaySize = EditorRenderEngineInterop.GetEditorSize();
            io.DisplayFramebufferScale = Vector2.One;
            io.DeltaTime = 1f / 60f;

            io.Fonts.GetTexDataAsRGBA32(out IntPtr pixels, out int width, out int height, out int bytesPerPixel);
            void* texId = EditorRenderEngineInterop.CreateImGuiEditor((void*)pixels, width, height, bytesPerPixel);
            io.Fonts.SetTexID((IntPtr)texId);
            io.Fonts.ClearTexData();

            ImGuizmo.SetImGuiContext(ImGui.GetCurrentContext());

            var style = ImGui.GetStyle();

            for (int i = 0; i < (int)ImGuiCol.COUNT; i++)
            {
                var color = style.Colors[i];
                color.W = 1f;
                style.Colors[i] = color;
            }

            AssetDataBase.Initialize(rootPath);
            EditorWindowEventInterop.AddFocusCallback(OnFocusChanged);
        }

        public static unsafe void Update()
        {
            _input.ProcessInput();

            ImGui.NewFrame();

            ImGui.PushStyleColor(ImGuiCol.WindowBg, Vector4.Zero);

            var centerId = ImGui.DockSpaceOverViewport(0, ImGui.GetMainViewport(), ImGuiDockNodeFlags.NoDockingOverCentralNode | ImGuiDockNodeFlags.PassthruCentralNode);
            ImGui.SetNextWindowDockID(centerId, ImGuiCond.Always);
            ImGui.Begin("Scene");

            ImGui.Text(Input.Runtime.MousePosition.ToString());

            RenderEngineInterop.MoveAndResize((int)ImGui.GetWindowPos().X, (int)ImGui.GetWindowPos().Y, (int)ImGui.GetWindowSize().X, (int)ImGui.GetWindowSize().Y);
            ImGui.PopStyleColor();
            ImGui.End();

            ImGui.ShowDemoWindow();

            if (ImGui.Begin("Resources Window"))
            {
                ImGui.Text("List of Resources:");

                foreach (var resource in AssetDataBase.AllAssets)
                    ImGui.Button(Path.GetFileName(resource.Value));

                ImGui.End();
            }

            ImGui.Render();

            EditorRenderEngineInterop.UpdateImGui(ImGui.GetDrawData().NativePtr);
        }

        public static void Destroy()
        {
            ImGuizmo.SetImGuiContext(0);
            ImGui.DestroyContext(ImGui.GetCurrentContext());

            EditorWindowEventInterop.RemoveFocusCallback(OnFocusChanged);
            AssetDataBase.Dispose();
        }

        private static void OnFocusChanged(bool focus)
        {
            if (focus && AssetDataBase.IsDirty)
                AssetDataBase.Resolve();
        }
    }
}

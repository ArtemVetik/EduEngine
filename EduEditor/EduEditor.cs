using ImGuiNET;
using ImGuizmoNET;
using System.Numerics;

namespace EduEngine.Editor
{
    public class EduEditor
    {
        private static ImGuiInput _input = new ImGuiInput();
        private static EditorCamera _camera = new EditorCamera();
        private static SceneData _sceneData;
        private static HierarchyWindow _hierarchyWindow = new HierarchyWindow();
        private static PropertyWindow _propertyWindow = new PropertyWindow();
        private static GuizmoRenderer _gizmoRenderer = new GuizmoRenderer();
        private static AssetWindow _assetWindow = new AssetWindow();
        private static AssetInfoWindow _assetInfo = new AssetInfoWindow();

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
            ImGuizmo.BeginFrame();

            ImGui.PushStyleColor(ImGuiCol.WindowBg, Vector4.Zero);

            var centerId = ImGui.DockSpaceOverViewport(0, ImGui.GetMainViewport(), ImGuiDockNodeFlags.NoDockingOverCentralNode | ImGuiDockNodeFlags.PassthruCentralNode);
            ImGui.SetNextWindowDockID(centerId, ImGuiCond.Always);
            ImGui.Begin("Scene", ImGuiWindowFlags.NoBackground);

            ImGui.Text(Input.Runtime.MousePosition.ToString());

            ImGuizmo.SetDrawlist(ImGui.GetWindowDrawList());

            var x = (int)ImGui.GetWindowPos().X;
            var y = (int)ImGui.GetWindowPos().Y;
            var w = (int)ImGui.GetWindowSize().X;
            var h = (int)ImGui.GetWindowSize().Y;

            _gizmoRenderer.Render(_hierarchyWindow.Selected, _camera, x, y, w, h);

            RenderEngineInterop.MoveAndResize(x, y, w, h);
            ImGui.PopStyleColor();

            ImGui.End();

            ImGui.ShowDemoWindow();

            _assetWindow.Render();
            _assetInfo.Render(_assetWindow.SelectedAsset);

            _hierarchyWindow.Render();
            _propertyWindow.Render(_hierarchyWindow.Selected);

            ImGui.BeginMainMenuBar();

            if (ImGui.Button("T"))
                _gizmoRenderer.SetOperation(OPERATION.TRANSLATE);
            if (ImGui.Button("R"))
                _gizmoRenderer.SetOperation(OPERATION.ROTATE);
            if (ImGui.Button("S"))
                _gizmoRenderer.SetOperation(OPERATION.SCALE);

            float panel_width = ImGui.GetContentRegionAvail().X;
            float button_width = 100.0f;

            float offset_x = (panel_width - button_width) * 0.5f;

            if (offset_x > 0.0f)
            {
                ImGui.SetCursorPosX(ImGui.GetCursorPosX() + offset_x);
            }

            bool disabled = false;
            if (EngineStateManager.CurrentState == EngineState.Runtime || SceneManager.CurrentScene == null)
            {
                ImGui.BeginDisabled();
                disabled = true;
            }

            if (ImGui.Button("Play", new Vector2(button_width, 0)))
            {
                EngineStateManager.CurrentState = EngineState.Runtime;
                _sceneData = SceneImporter.ToSceneData(SceneManager.CurrentScene);
                SceneImporter.LoadScene(_sceneData, SceneManager.CurrentScene.GUID, true);
            }

            if (disabled)
                ImGui.EndDisabled();

            disabled = false;
            if (EngineStateManager.CurrentState == EngineState.Editor)
            {
                ImGui.BeginDisabled();
                disabled = true;
            }

            if (ImGui.Button("Stop", new Vector2(button_width, 0)))
            {
                EngineStateManager.CurrentState = EngineState.Editor;
                SceneImporter.LoadScene(_sceneData, SceneManager.CurrentScene.GUID, false);
            }

            if (disabled)
                ImGui.EndDisabled();

            ImGui.EndMainMenuBar();

            ImGui.Render();

            EditorRenderEngineInterop.UpdateImGui(ImGui.GetDrawData().NativePtr);
        }

        public static void RenderScene()
        {
            _camera.Update();
            _camera.Render();
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

using ImGuiNET;
using ImGuizmoNET;
using System.Numerics;

namespace EduEngine.Editor
{
    public static class EduEditor
    {
        private static ImGuiInput _input = new ImGuiInput();
        private static EditorCamera _camera = new EditorCamera();
        private static HierarchyWindow _hierarchyWindow = new HierarchyWindow();
        private static PropertyWindow _propertyWindow = new PropertyWindow();
        private static GuizmoRenderer _guizmoRenderer = new GuizmoRenderer();
        private static SceneWindow _sceneWindow = new SceneWindow(_guizmoRenderer, _hierarchyWindow, _camera);
        private static AssetWindow _assetWindow = new AssetWindow();
        private static AssetInfoWindow _assetInfo = new AssetInfoWindow();
        private static RenderResourcesInfo _renderResourcesInfo = new RenderResourcesInfo();
        private static MainMenuBar _menuBar = new MainMenuBar(_renderResourcesInfo, _hierarchyWindow);

        private static bool _withEditor;

        public static unsafe void Initialize(string assetsPath, string dllPath, bool withEditor = true)
        {
            _withEditor = withEditor;

            AssetDataBase.Initialize(assetsPath, dllPath);

            if (_withEditor)
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
                IntPtr texId = EditorRenderEngineInterop.CreateImGuiEditor(pixels, width, height, bytesPerPixel);
                io.Fonts.SetTexID(texId);
                io.Fonts.ClearTexData();

                ImGuizmo.SetImGuiContext(ImGui.GetCurrentContext());
                EditorWindowEventInterop.AddFocusCallback(OnFocusChanged);
            }
            else
            {
                var mainScene = AssetDataBase.AllAssets.FirstOrDefault(asset =>
                    asset.Value.Type == AssetType.Scene &&
                    Path.GetFileNameWithoutExtension(asset.Value.LocalPath) == "MainScene");

                if (string.IsNullOrEmpty(mainScene.Key) == false)
                    SceneImporter.LoadScene(mainScene.Key, true);
            }
        }

        public static void Update()
        {
            _input.ProcessInput();

            ImGui.NewFrame();
            ImGuizmo.BeginFrame();

            _sceneWindow.Render(out Vector2 pos, out Vector2 size);
            _guizmoRenderer.Render(_hierarchyWindow.Selected, _camera, pos.X, pos.Y, size.X, size.Y);
            ImGui.ShowDemoWindow();
            _assetWindow.Render();
            _assetInfo.Render(_assetWindow.SelectedAsset);
            _hierarchyWindow.Render();
            _propertyWindow.Render(_hierarchyWindow.Selected);
            _renderResourcesInfo.Render();
            _menuBar.Render();

            ImGui.Render();
            ImGui.GetDrawData();

            unsafe
            {
                EditorRenderEngineInterop.UpdateImGui(new IntPtr(ImGui.GetDrawData().NativePtr));
            }
        }

        public static void RenderScene()
        {
            _camera.Update();
            _camera.Render();
        }

        public static void Destroy()
        {
            if (_withEditor)
            {
                ImGuizmo.SetImGuiContext(0);
                ImGui.DestroyContext(ImGui.GetCurrentContext());

                EditorWindowEventInterop.RemoveFocusCallback(OnFocusChanged);
            }

            _renderResourcesInfo.Dispose();
            AssetDataBase.Dispose();
        }

        private static void OnFocusChanged(bool focus)
        {
            if (focus && AssetDataBase.IsDirty)
                AssetDataBase.Resolve();
        }
    }
}

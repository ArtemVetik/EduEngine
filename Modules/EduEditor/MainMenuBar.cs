using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class MainMenuBar
    {
        private readonly RenderResourcesInfo _renderResourcesInfo;
        private readonly HierarchyWindow _hierarchyWindow;
        private readonly RenderSettingsWindow _renderSettingsWindow;

        private SceneData? _sceneData = null;

        public MainMenuBar(RenderResourcesInfo renderResourcesInfo, HierarchyWindow hierarchyWindow)
        {
            _renderResourcesInfo = renderResourcesInfo;
            _hierarchyWindow = hierarchyWindow;
            _renderSettingsWindow = new RenderSettingsWindow();
        }

        public void Render()
        {
            ImGui.BeginMainMenuBar();

            if (ImGui.BeginMenu("Windows"))
            {
                if (ImGui.MenuItem("Render Objects Stats"))
                {
                    _renderResourcesInfo.Show = true;
                }
                ImGui.EndMenu();
            }
            if (ImGui.BeginMenu("Settings"))
            {
                if (ImGui.MenuItem("Render Settings"))
                {
                    _renderSettingsWindow.Show = true;
                }
                ImGui.EndMenu();
            }

            float panel_width = ImGui.GetContentRegionAvail().X;
            float button_width = 100.0f;

            float offset_x = (panel_width - button_width * 2) * 0.5f;

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
                EngineStateManager.InspectScene = false;
                _hierarchyWindow.ResetSelected();
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
                _hierarchyWindow.ResetSelected();
                EngineStateManager.CurrentState = EngineState.Editor;
                SceneImporter.LoadScene(_sceneData, SceneManager.CurrentScene.GUID, false);
            }

            if (disabled)
                ImGui.EndDisabled();

            ImGui.EndMainMenuBar();


            _renderSettingsWindow.Render();
        }
    }
}

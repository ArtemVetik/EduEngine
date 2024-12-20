using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class MainMenuBar
    {
        private readonly RenderResourcesInfo _renderResourcesInfo;

        private SceneData? _sceneData = null;

        public MainMenuBar(RenderResourcesInfo renderResourcesInfo)
        {
            _renderResourcesInfo = renderResourcesInfo;
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
        }
    }
}

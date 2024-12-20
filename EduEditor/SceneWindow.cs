using ImGuiNET;
using ImGuizmoNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class SceneWindow
    {
        private GuizmoRenderer _guizmoRenderer;
        private HierarchyWindow _hierarhyWindow;
        private bool _debugDraw = true;

        public SceneWindow(GuizmoRenderer guizmoRenderer, HierarchyWindow hierarhyWindow)
        {
            _guizmoRenderer = guizmoRenderer;
            _hierarhyWindow = hierarhyWindow;

        }

        public bool DebugDraw => _debugDraw;

        public void Render(out Vector2 pos, out Vector2 size)
        {
            ImGui.PushStyleColor(ImGuiCol.WindowBg, Vector4.Zero);

            var centerId = ImGui.DockSpaceOverViewport(0, ImGui.GetMainViewport(), ImGuiDockNodeFlags.NoDockingOverCentralNode | ImGuiDockNodeFlags.PassthruCentralNode);
            ImGui.SetNextWindowDockID(centerId, ImGuiCond.Always);
            
            ImGui.Begin("Scene", ImGuiWindowFlags.NoBackground | ImGuiWindowFlags.MenuBar);

            ImGui.BeginMenuBar();

            if (EngineStateManager.CurrentState == EngineState.Editor)
            {
                bool AviableHotKey(KeyCode key) =>
                     _hierarhyWindow.Selected != null && Input.Editor.KeyDown(key) && !Input.Editor.MouseButtonPressed(MouseCode.Mouse1);

                if (ImGui.MenuItem("Translate", "", _guizmoRenderer.Operation == OPERATION.TRANSLATE) || AviableHotKey(KeyCode.W))
                    _guizmoRenderer.Operation = OPERATION.TRANSLATE;
                if (ImGui.MenuItem("Rotate", "", _guizmoRenderer.Operation == OPERATION.ROTATE) || AviableHotKey(KeyCode.E))
                    _guizmoRenderer.Operation = OPERATION.ROTATE;
                if (ImGui.MenuItem("Scale", "", _guizmoRenderer.Operation == OPERATION.SCALE) || AviableHotKey(KeyCode.R))
                    _guizmoRenderer.Operation = OPERATION.SCALE;

                ImGui.Spacing();

                if (ImGui.MenuItem("World", "", _guizmoRenderer.Mode == MODE.WORLD))
                    _guizmoRenderer.Mode = MODE.WORLD;
                if (ImGui.MenuItem("Local", "", _guizmoRenderer.Mode == MODE.LOCAL))
                    _guizmoRenderer.Mode = MODE.LOCAL;

                ImGui.Spacing();

                ImGui.Checkbox("Debug Draw", ref _debugDraw);
            }
            else if (EngineStateManager.CurrentState == EngineState.Runtime)
            {
                ImGui.Text("(Runtime)");
            }

            ImGui.EndMenuBar();

            ImGui.Text(Input.Runtime.MousePosition.ToString());

            pos = ImGui.GetWindowPos();
            size = ImGui.GetWindowSize();

            ImGuizmo.SetDrawlist(ImGui.GetWindowDrawList());
            ImGui.End();

            RenderEngineInterop.MoveAndResize((int)pos.X, (int)pos.Y, (int)size.X, (int)size.Y);
            ImGui.PopStyleColor();
        }
    }
}

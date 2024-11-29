using ImGuiNET;

namespace EduEngine.Editor
{
    internal class PropertyWindow
    {
        public void Render(GameObject selected)
        {
            if (selected == null)
            {
                ImGui.Begin("PropertyWindow");
                ImGui.End();
                return;
            }

            ImGui.Begin("PropertyWindow");

            var localPos = selected.Transform.Position;
            var eulerAngles = selected.Transform.Rotation.ToEuler();
            var localScale = selected.Transform.LocalScale;

            ImGui.InputText("Name", ref selected.Name, 64);

            if (ImGui.InputFloat3("Local Position", ref localPos))
                selected.Transform.Position = localPos;
            if (ImGui.InputFloat3("Local Rotation", ref eulerAngles))
                selected.Transform.Rotation = eulerAngles.ToEulerQuaternion();
            if (ImGui.InputFloat3("Local Scale", ref localScale))
                selected.Transform.LocalScale = localScale;

            ImGui.End();
        }
    }
}

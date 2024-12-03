using ImGuiNET;

namespace EduEngine.Editor
{
    internal class HierarchyWindow
    {
        public GameObject Selected { get; private set; }

        public void Render()
        {
            ImGui.Begin("Hierarchy");

            if (SceneManager.CurrentScene.Objects.Count < 3)
            {
                if (ImGui.IsWindowHovered(ImGuiHoveredFlags.None) &&
                    ImGui.IsMouseReleased(ImGuiMouseButton.Right) &&
                    !ImGui.IsAnyItemHovered())
                {
                    ImGui.OpenPopup("HierarchyContextMenu");
                }

                if (ImGui.BeginPopup("HierarchyContextMenu"))
                {
                    if (ImGui.BeginMenu("Create"))
                    {
                        if (ImGui.MenuItem("GameObject"))
                        {
                            new EditorGameObject();
                        }

                        ImGui.EndMenu();
                    }

                    ImGui.EndPopup();
                }
            }

            var deleteItems = new List<GameObject>();

            if (ImGui.TreeNode("SceneTree"))
            {
                foreach (var item in SceneManager.CurrentScene.Objects)
                {
                    if (ImGui.Selectable(item.Name + "##" + item.Guid.ToString(), item == Selected))
                    {
                        Selected = item;
                        ImGui.SetWindowFocus("PropertyWindow");
                    }
                    if (item == Selected && ImGui.IsItemClicked(ImGuiMouseButton.Right))
                    {
                        ImGui.OpenPopup("HierarchyItem##" + item.Guid.ToString());
                    }

                    if (ImGui.BeginPopup("HierarchyItem##" + item.Guid.ToString()))
                    {
                        if (ImGui.MenuItem("Delete"))
                        {
                            deleteItems.Add(item);
                        }
                        ImGui.EndPopup();
                    }
                }

                ImGui.TreePop();
            }

            if (ImGui.IsWindowHovered(ImGuiHoveredFlags.AllowWhenBlockedByPopup) &&
                ImGui.IsMouseReleased(ImGuiMouseButton.Left) &&
                !ImGui.IsAnyItemHovered())
            {
                Selected = null;
            }

            foreach (var item in deleteItems)
                item.Destroy();

            ImGui.End();
        }
    }
}

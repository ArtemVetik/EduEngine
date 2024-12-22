using ImGuiNET;

namespace EduEngine.Editor
{
    internal class HierarchyWindow
    {
        public GameObject? Selected { get; private set; }

        public void ResetSelected() => Selected = null;

        public void Render()
        {
            ImGui.Begin("Hierarchy");

            if (SceneManager.CurrentScene == null)
            {
                ImGui.Text("Scene not loaded");
                ImGui.End();
                return;
            }

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

                if (EngineStateManager.CurrentState != EngineState.Editor)
                    ImGui.BeginDisabled();

                if (ImGui.MenuItem("Save Scene"))
                {
                    SceneImporter.SaveCurrentScene();
                }

                if (EngineStateManager.CurrentState != EngineState.Editor)
                    ImGui.EndDisabled();

                ImGui.EndPopup();
            }

            var deleteItems = new List<GameObject>();

            if (AssetDataBase.HasGUID(SceneManager.CurrentScene.GUID))
            {
                var path = AssetDataBase.GetAssetData(SceneManager.CurrentScene.GUID).LocalPath;
                ImGui.Text(Path.GetFileName(path));
            }
            else
            {
                ImGui.Text("New Scene");
            }

            if (ImGui.TreeNode("SceneTree"))
            {
                foreach (var item in SceneManager.CurrentScene.Objects)
                {
                    if (item.Parent != null)
                        continue;

                    RenderGameObjectTree(item, ref deleteItems);
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

        private void RenderGameObjectTree(GameObject go, ref List<GameObject> deleteItems)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags.OpenOnArrow;

            if (go.Childs.Count == 0)
                flags |= ImGuiTreeNodeFlags.Leaf;

            if (go == Selected)
                flags |= ImGuiTreeNodeFlags.Selected;

            bool open = ImGui.TreeNodeEx(go.Name + "##" + go.Guid.ToString(), flags);

            if (ImGui.IsItemClicked())
            {
                Selected = go;
                ImGui.SetWindowFocus("PropertyWindow");
            }

            if (ImGui.IsItemClicked(ImGuiMouseButton.Right))
            {
                ImGui.OpenPopup("HierarchyItem##" + go.Guid.ToString());
            }

            if (ImGui.BeginPopup("HierarchyItem##" + go.Guid.ToString()))
            {
                if (ImGui.MenuItem("Delete"))
                {
                    deleteItems.Add(go);
                }
                ImGui.EndPopup();
            }

            if (open)
            {
                foreach (var child in go.Childs)
                {
                    RenderGameObjectTree(child, ref deleteItems);
                }
                ImGui.TreePop();
            }
        }
    }
}

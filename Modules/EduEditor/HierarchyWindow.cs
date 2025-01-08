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
            var copyItems = new List<GameObject>();

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

                    RenderGameObjectTree(item, ref deleteItems, ref copyItems);
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

            foreach (var item in copyItems)
                CopyGameObject(item);

            ImGui.End();
        }

        private void RenderGameObjectTree(GameObject go, ref List<GameObject> deleteItems, ref List<GameObject> copyItems)
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
                if (ImGui.MenuItem("Copy"))
                {
                    copyItems.Add(go);
                }
                ImGui.EndPopup();
            }

            if (open)
            {
                foreach (var child in go.Childs)
                {
                    RenderGameObjectTree(child, ref deleteItems, ref copyItems);
                }
                ImGui.TreePop();
            }
        }

        private void CopyGameObject(GameObject reference)
        {
            var obj = new EditorGameObject();
            obj.SetParent(reference.Parent);
            obj.Transform.LocalPosition = reference.Transform.LocalPosition;
            obj.Transform.LocalRotation = reference.Transform.LocalRotation;
            obj.Transform.LocalScale = reference.Transform.LocalScale;
            obj.Name = reference.Name + " Copy";

            var components = reference.GetComponents<Component>();
            foreach (var component in components)
            {
                var fields = ComponentFields.FindAll(component);

                obj.AddComponent(component.GetType(), (c) =>
                {
                    foreach (var field in fields)
                    {
                        var objField = ComponentFields.FindField(c.GetType(), field.Name);
                        var value = field.GetValue(component);
                        objField.SetValue(c, value);
                    }
                });
            }
        }
    }
}

using ImGuiNET;

namespace EduEngine.Editor
{
    internal class HierarchyWindow
    {
        private class DragNode
        {
            public GameObject Go;
        }

        internal HierarchyWindow(EditorCamera camera)
        {
            _camera = camera;
        }

        private EditorCamera _camera;
        private DragNode _draggingNode = null;
        private DragNode _dropTargetNode = null;
        private float _lastClick = -1f;

        private GameObject _deleteItem = null;
        private GameObject _copyItem = null;
        private (GameObject, GameObject?) _moveItem = (null, null);

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

            if (EngineStateManager.CurrentState == EngineState.Editor)
                PickObject();

            RenderHierarhyPopup();
            RenderSceneTree();

            if (ClickOnEmptyArea())
                Selected = null;

            ImGui.End();
        }

        private void RenderSceneTree()
        {
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
                HandleDragAndDrop(new DragNode() { Go = null });

                foreach (var item in SceneManager.CurrentScene.Objects)
                {
                    if (item.Parent != null)
                        continue;

                    RenderGameObjectTree(item);
                }

                ImGui.TreePop();
            }

            if (_deleteItem != null)
                _deleteItem.Destroy();

            if (_copyItem != null)
                CopyGameObjectHierarhy(_copyItem, _copyItem.Parent);

            if (_moveItem.Item1 != null)
                _moveItem.Item1.SetParent(_moveItem.Item2, true);

            _deleteItem = null;
            _copyItem = null;
            _moveItem = (null, null);
        }

        private void RenderGameObjectTree(GameObject go)
        {
            bool ChildSelected(GameObject go)
            {
                if (go == Selected)
                    return true;

                foreach (var child in go.Childs)
                    if (ChildSelected(child))
                        return true;

                return false;
            }

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags.OpenOnArrow;

            if (go.Childs.Count == 0)
                flags |= ImGuiTreeNodeFlags.Leaf;

            if (go == Selected)
                flags |= ImGuiTreeNodeFlags.Selected;

            if (ChildSelected(go))
                flags |= ImGuiTreeNodeFlags.DefaultOpen;

            bool open = ImGui.TreeNodeEx(go.Name + "##" + go.Guid.ToString(), flags);

            if (ImGui.IsItemClicked())
                Selected = go;

            if (ImGui.IsItemClicked(ImGuiMouseButton.Left) && ImGui.IsMouseDoubleClicked(ImGuiMouseButton.Left))
            {
                var maxScale = MathF.Max(MathF.Abs(go.Transform.LossyScale.X),
                    MathF.Max(MathF.Abs(go.Transform.LossyScale.Y), MathF.Abs(go.Transform.LossyScale.Z)));

                var offset = _camera.Forward * (maxScale + 5);
                _camera.SetTarget(go.Transform.Position - offset);
            }

            HandleDragAndDrop(new DragNode() { Go = go });

            if (ImGui.IsItemClicked(ImGuiMouseButton.Right))
            {
                ImGui.OpenPopup("HierarchyItem##" + go.Guid.ToString());
            }

            if (ImGui.BeginPopup("HierarchyItem##" + go.Guid.ToString()))
            {
                if (ImGui.MenuItem("Delete"))
                {
                    _deleteItem = go;
                }
                if (ImGui.MenuItem("Copy"))
                {
                    _copyItem = go;
                }
                ImGui.EndPopup();
            }

            if (open)
            {
                foreach (var child in go.Childs)
                {
                    RenderGameObjectTree(child);
                }
                ImGui.TreePop();
            }
        }

        private unsafe void HandleDragAndDrop(DragNode node)
        {
            if (node.Go != null && ImGui.BeginDragDropSource())
            {
                _draggingNode = node;
                ImGui.SetDragDropPayload("TREE_NODE", IntPtr.Zero, 0);
                ImGui.Text(node.Go.Name);
                ImGui.EndDragDropSource();
            }
            if (ImGui.BeginDragDropTarget())
            {
                var payload = ImGui.AcceptDragDropPayload("TREE_NODE");
                if (payload.NativePtr != null && _draggingNode != null && _draggingNode != node)
                {
                    _dropTargetNode = node;
                }
                ImGui.EndDragDropTarget();
            }
            if (_draggingNode != null && _dropTargetNode != null)
            {
                MoveGameObject(_draggingNode.Go, _dropTargetNode.Go);
                _draggingNode = null;
                _dropTargetNode = null;
            }
        }

        private void MoveGameObject(GameObject go, GameObject? newParent)
        {
            var parent = newParent?.Parent;
            while (parent != null)
            {
                if (parent == go)
                    return;

                parent = parent.Parent;
            }

            _moveItem = (go, newParent);
        }

        private bool ClickOnEmptyArea()
        {
            return ImGui.IsWindowHovered(ImGuiHoveredFlags.AllowWhenBlockedByPopup) &&
                    ImGui.IsMouseReleased(ImGuiMouseButton.Left) &&
                    !ImGui.IsAnyItemHovered();
        }
        private void RenderHierarhyPopup()
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
        }
        private void CopyGameObjectHierarhy(GameObject reference, GameObject parent)
        {
            var obj = new EditorGameObject();
            obj.SetParent(parent);
            obj.Transform.LocalPosition = reference.Transform.LocalPosition;
            obj.Transform.LocalRotation = reference.Transform.LocalRotation;
            obj.Transform.LocalScale = reference.Transform.LocalScale;
            obj.Name = reference.Name;

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

            var childsCopy = reference.Childs.ToArray();

            foreach (var child in childsCopy)
                CopyGameObjectHierarhy(child, obj);
        }
        private void PickObject()
        {
            if (Input.Editor.MouseButtonDown(0) && SceneManager.CurrentScene != null)
            {
                if (Input.Runtime.MousePosition.X < 0 || Input.Runtime.MousePosition.X > Screen.Size.X ||
                    Input.Runtime.MousePosition.Y < 0 || Input.Runtime.MousePosition.Y > Screen.Size.Y)
                {
                    return;
                }

                if (EditorTime.TotalTime - _lastClick > 0.4f)
                {
                    _lastClick = EditorTime.TotalTime;
                    return;
                }

                float minDist = float.MaxValue;

                foreach (var go in SceneManager.CurrentScene.Objects)
                {
                    var renderer = go.GetComponent<Renderer>();

                    if (renderer == null)
                        continue;

                    float pickDistance;
                    if (RayPickingInterop.Intersect(renderer.GetWrapper(), _camera.GetWrapper(), Screen.Size, Input.Runtime.MousePosition, out pickDistance))
                    {
                        if (pickDistance < minDist)
                        {
                            Selected = go;
                            minDist = pickDistance;
                        }
                    }
                }
            }
        }
    }
}

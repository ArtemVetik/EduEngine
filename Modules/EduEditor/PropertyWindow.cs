using ImGuiNET;
using Microsoft.CodeAnalysis;
using System.Reflection;

namespace EduEngine.Editor
{
    internal class PropertyWindow
    {
        private static string _scriptFilter = "";

        public void Render(GameObject? selected)
        {
            if (selected == null)
            {
                ImGui.Begin("PropertyWindow");
                ImGui.Text("No GameObject has been selected");
                ImGui.End();
                return;
            }

            ImGui.Begin("PropertyWindow");

            ImGui.InputText("Name", ref selected.Name, 64);
            RenderParent(selected);
            RenderTransform(selected);
            RenderComponents(selected);

            ImGui.SeparatorText("");

            RenderAttachScript(selected);

            ImGui.End();
        }

        private void RenderParent(GameObject go)
        {
            var ignoreParents = new List<GameObject>() { go };

            void AddChilds(GameObject go, ref List<GameObject> childs)
            {
                foreach (var child in go.Childs)
                {
                    childs.Add(child);
                    if (child.Childs.Count > 0)
                        AddChilds(child, ref childs);
                }
            }

            AddChilds(go, ref ignoreParents);

            ImGuiEx.RenderGameObjectSelect(go.Parent, "Parent", (selected) =>
            {
                go.SetParent(selected, true);
            }, ignoreParents);
        }

        private void RenderTransform(GameObject go)
        {
            var localPos = go.Transform.LocalPosition;
            var eulerAngles = go.Transform.LocalRotation.ToEuler();
            var localScale = go.Transform.LocalScale;

            if (ImGui.InputFloat3("Local Position", ref localPos))
                go.Transform.LocalPosition = localPos;
            if (ImGui.InputFloat3("Local Rotation", ref eulerAngles))
                go.Transform.LocalRotation = eulerAngles.ToEulerQuaternion();
            if (ImGui.InputFloat3("Local Scale", ref localScale))
                go.Transform.LocalScale = localScale;
        }

        private void RenderComponents(GameObject go)
        {
            var components = go.GetComponents<Component>();

            foreach (var component in components)
            {
                ImGui.Separator();

                if (ImGui.CollapsingHeader($"{component}##header{component.GUID}"))
                {
                    ImGui.Indent();

                    RenderComponentFields(component);

                    ImGui.Spacing();
                    ImGui.Separator();
                    ImGui.Spacing();

                    if (ImGui.Button($"Remove Component##button{component.GUID}"))
                    {
                        go.RemoveComponent(component);
                    }

                    ImGui.Unindent();
                }
            }
        }

        private void RenderComponentFields(Component component)
        {
            var fields = ComponentFields.FindAll(component);

            foreach (var field in fields)
            {
                PropertyFieldRenderer.RenderField(field, component);
            }
        }

        private void RenderAttachScript(GameObject go)
        {
            var assemblies = new Assembly[]
            {
                Assembly.Load("GameplayFramework"),
                Assembly.LoadFrom(AssetDataBase.DllPath),
            };
            var derivedTypes = new List<Type>();

            foreach (var assembly in assemblies)
            {
                try
                {
                    var types = assembly.GetTypes();

                    derivedTypes.AddRange(types.Where(t =>
                        t.IsClass &&
                        !t.IsAbstract &&
                        t.IsSubclassOf(typeof(Component))));
                }
                catch (ReflectionTypeLoadException ex)
                {
                    foreach (var loaderException in ex.LoaderExceptions)
                    {
                        Console.WriteLine(loaderException);
                    }
                }
            }

            if (ImGui.Button("Attach Script"))
                ImGui.OpenPopup("SelectScriptPopup");

            if (ImGui.BeginPopup("SelectScriptPopup"))
            {
                ImGui.InputText("Search", ref _scriptFilter, 256);

                ImGui.Separator();

                foreach (var option in derivedTypes)
                {
                    if (string.IsNullOrEmpty(_scriptFilter) || option.Name.Contains(_scriptFilter, StringComparison.OrdinalIgnoreCase))
                    {
                        if (ImGui.Selectable(option.Name))
                        {
                            go.AddComponent(option);

                            _scriptFilter = "";
                            ImGui.CloseCurrentPopup();
                        }
                    }
                }

                ImGui.EndPopup();
            }
        }
    }
}

using ImGuiNET;
using Microsoft.CodeAnalysis;
using System.Reflection;

namespace EduEngine.Editor
{
    internal class PropertyWindow
    {
        private static string _goFilter = "";
        private static string _scriptFilter = "";

        public void Render(GameObject selected)
        {
            if (selected == null)
            {
                ImGui.Begin("PropertyWindow");
                ImGui.End();
                return;
            }

            ImGui.Begin("PropertyWindow");

            RenderTransform(selected);
            RenderComponents(selected);

            ImGui.SeparatorText("");

            RenderAttachScript(selected);

            ImGui.End();
        }

        private void RenderTransform(GameObject go)
        {
            var localPos = go.Transform.LocalPosition;
            var eulerAngles = go.Transform.LocalRotation.ToEuler();
            var localScale = go.Transform.LocalScale;

            ImGui.InputText("Name", ref go.Name, 64);

            if (ImGui.BeginCombo("Parent", go.Parent == null ? "null" : go.Parent.Name))
            {
                ImGui.InputText("Search", ref _goFilter, 256);

                ImGui.Separator();

                if (ImGui.Selectable("null"))
                {
                    go.SetParent(null);
                    _goFilter = "";
                    ImGui.CloseCurrentPopup();
                }

                foreach (var option in FilterGameObjects(go))
                {
                    if (ImGui.Selectable(option.Name))
                    {
                        go.SetParent(option);
                        _goFilter = "";
                        ImGui.CloseCurrentPopup();
                    }
                }

                ImGui.EndCombo();
            }

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
                var fieldName = field.Name + $"##field{component.GUID}";
                var fieldValue = field.GetValue(component);

                if (fieldValue is int intValue)
                {
                    int newValue = intValue;
                    if (ImGui.InputInt(fieldName, ref newValue))
                    {
                        ReflectField.Set(field, component, newValue);
                    }
                }
                else if (fieldValue is float floatValue)
                {
                    float newValue = floatValue;
                    if (ImGui.InputFloat(fieldName, ref newValue))
                    {
                        ReflectField.Set(field, component, newValue);
                    }
                }
                else if (fieldValue is string stringValue)
                {
                    string newValue = stringValue;
                    if (ImGui.InputText(fieldName, ref newValue, 256))
                    {
                        ReflectField.Set(field, component, newValue);
                    }
                }
                else if (fieldValue is bool boolValue)
                {
                    bool newValue = boolValue;
                    if (ImGui.Checkbox(fieldName, ref newValue))
                    {
                        ReflectField.Set(field, component, newValue);
                    }
                }
                else if (fieldValue is Enum enumValue)
                {
                    Enum newValue = enumValue;
                    if (ImGui.BeginCombo(fieldName, enumValue.ToString()))
                    {
                        var names = Enum.GetNames(field.FieldType);
                        foreach (var name in names)
                        {
                            if (ImGui.Selectable(name))
                            {
                                ReflectField.Set(field, component, Enum.Parse(field.FieldType, name));
                            }
                        }

                        ImGui.EndCombo();
                    }
                }
                else if (field.FieldType.IsSubclassOf(typeof(Asset)))
                {
                    Asset currentAsset = fieldValue as Asset;

                    if (currentAsset != null && currentAsset.IsValid == false)
                    {
                        ReflectField.Set(field, component, null);
                    }
                    else
                    {
                        var assets = AssetDataBase.AllAssets.Where(asset => asset.Value.Asset != null &&
                                                                            asset.Value.Asset.GetType() == field.FieldType);

                        if (ImGui.BeginCombo(field.Name, currentAsset == null ? "null" : AssetDataBase.GetAssetData(currentAsset.GUID).LocalPath))
                        {
                            if (ImGui.Selectable("null"))
                            {
                                ReflectField.Set(field, component, null);
                            }

                            foreach (var asset in assets)
                            {
                                if (ImGui.Selectable(asset.Value.LocalPath))
                                {
                                    ReflectField.Set(field, component, asset.Value.Asset);
                                }
                            }
                            ImGui.EndCombo();
                        }
                    }
                }
                else
                {
                    ImGui.Text($"{field.Name}: Unsupported type ({field.FieldType.Name})");
                }
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

                foreach (var option in FilterScripts(derivedTypes))
                {
                    if (ImGui.Selectable(option.Name))
                    {
                        go.AddComponent(option);

                        _scriptFilter = "";
                        ImGui.CloseCurrentPopup();
                    }
                }

                ImGui.EndPopup();
            }
        }

        private static IEnumerable<Type> FilterScripts(List<Type> types)
        {
            for (int i = 0; i < types.Count; i++)
            {
                var option = types[i];

                if (string.IsNullOrEmpty(_scriptFilter) || option.Name.Contains(_scriptFilter, StringComparison.OrdinalIgnoreCase))
                    yield return option;
            }
        }

        private static IEnumerable<GameObject> FilterGameObjects(GameObject self)
        {
            foreach (var go in SceneManager.CurrentScene.Objects)
            {
                if (go == self)
                    continue;

                if (string.IsNullOrEmpty(_scriptFilter) || go.Name.Contains(_scriptFilter, StringComparison.OrdinalIgnoreCase))
                    yield return go;
            }
        }
    }
}

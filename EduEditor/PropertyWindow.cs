using ImGuiNET;
using Microsoft.CodeAnalysis;
using System.Reflection;

namespace EduEngine.Editor
{
    internal class PropertyWindow
    {
        private static string _filter = "";

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
            var localPos = go.Transform.Position;
            var eulerAngles = go.Transform.Rotation.ToEuler();
            var localScale = go.Transform.LocalScale;

            ImGui.InputText("Name", ref go.Name, 64);

            if (ImGui.InputFloat3("Local Position", ref localPos))
                go.Transform.Position = localPos;
            if (ImGui.InputFloat3("Local Rotation", ref eulerAngles))
                go.Transform.Rotation = eulerAngles.ToEulerQuaternion();
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

                    var publicFields = component.GetType().GetFields(BindingFlags.Public | BindingFlags.Instance);
                    var serializedFields = component.GetType().GetFields(BindingFlags.NonPublic | BindingFlags.Instance)
                        .Where(f => f.GetCustomAttribute<SerializeFieldAttribute>() != null);

                    var fields = publicFields.Concat(serializedFields);

                    var serializeCallback = component as ISerializeCallback;

                    foreach (var field in fields)
                    {
                        var fieldName = field.Name + $"##field{component.GUID}";
                        var fieldValue = field.GetValue(component);

                        if (fieldValue is int intValue)
                        {
                            int newValue = intValue;
                            if (ImGui.InputInt(fieldName, ref newValue))
                            {
                                field.SetValue(component, newValue);
                                serializeCallback?.OnDeserialize();
                            }
                        }
                        else if (fieldValue is float floatValue)
                        {
                            float newValue = floatValue;
                            if (ImGui.InputFloat(fieldName, ref newValue))
                            {
                                field.SetValue(component, newValue);
                                serializeCallback?.OnDeserialize();
                            }
                        }
                        else if (fieldValue is string stringValue)
                        {
                            string newValue = stringValue;
                            if (ImGui.InputText(fieldName, ref newValue, 256))
                            {
                                field.SetValue(component, newValue);
                                serializeCallback?.OnDeserialize();
                            }
                        }
                        else if (field.FieldType.IsSubclassOf(typeof(Asset)))
                        {
                            Asset currentAsset = fieldValue as Asset;

                            if (currentAsset != null && currentAsset.IsValid == false)
                            {
                                field.SetValue(component, null);
                                serializeCallback?.OnDeserialize();
                            }
                            else
                            {
                                var assets = AssetDataBase.AllAssets.Where(asset => asset.Value.Asset != null &&
                                                                                    asset.Value.Asset.GetType() == field.FieldType);

                                if (ImGui.BeginCombo(field.Name, currentAsset == null ? "null" : AssetDataBase.GetAssetData(currentAsset.GUID).LocalPath))
                                {
                                    if (ImGui.Selectable("null"))
                                    {
                                        field.SetValue(component, null);
                                        serializeCallback?.OnDeserialize();
                                    }

                                    foreach (var asset in assets)
                                    {
                                        if (ImGui.Selectable(asset.Value.LocalPath))
                                        {
                                            field.SetValue(component, asset.Value.Asset);
                                            serializeCallback?.OnDeserialize();
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

                    if (ImGui.Button($"Remove Component##button{component.GUID}"))
                    {
                        go.RemoveComponent(component);
                    }

                    ImGui.Unindent();
                }
            }
        }

        private void RenderAttachScript(GameObject go)
        {
            var assemblies = new Assembly[]
            {
                Assembly.Load("GameplayFramework"),
                Assembly.Load("Scripts"),
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
                ImGui.InputText("Search", ref _filter, 256);

                ImGui.Separator();

                foreach (var option in FilterScripts(derivedTypes))
                {
                    if (ImGui.Selectable(option.Name))
                    {
                        go.AddComponent(option);

                        _filter = "";
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

                if (string.IsNullOrEmpty(_filter) || option.Name.Contains(_filter, StringComparison.OrdinalIgnoreCase))
                    yield return option;
            }
        }
    }
}

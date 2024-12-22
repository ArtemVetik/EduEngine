using ImGuiNET;
using System.Numerics;
using System.Reflection;

namespace EduEngine.Editor
{
    internal static class PropertyFieldRenderer
    {
        public static void RenderField(FieldInfo field, Component component)
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
            else if (fieldValue is Vector2 vector2Value)
            {
                Vector2 newValue = vector2Value;
                if (field.HasAttribute(out RangeAttribute range) && ImGui.SliderFloat2(fieldName, ref newValue, range.Min, range.Max))
                {
                    ReflectField.Set(field, component, newValue);
                }
                else if (ImGui.DragFloat2(fieldName, ref newValue))
                {
                    ReflectField.Set(field, component, newValue);
                }
            }
            else if (fieldValue is Vector3 vector3Value)
            {
                Vector3 newValue = vector3Value;
                if (field.HasAttribute(out ColorAttribute _) && ImGui.ColorEdit3(fieldName, ref newValue))
                {
                    ReflectField.Set(field, component, newValue);
                }
                else if (field.HasAttribute(out RangeAttribute range) && ImGui.SliderFloat3(fieldName, ref newValue, range.Min, range.Max))
                {
                    ReflectField.Set(field, component, newValue);
                }
                else if (ImGui.DragFloat3(fieldName, ref newValue))
                {
                    ReflectField.Set(field, component, newValue);
                }
            }
            else if (fieldValue is Vector4 vector4Value)
            {
                Vector4 newValue = vector4Value;

                if (field.HasAttribute(out ColorAttribute _) && ImGui.ColorEdit4(fieldName, ref newValue))
                {
                    ReflectField.Set(field, component, newValue);
                }
                else if (field.HasAttribute(out RangeAttribute range) && ImGui.SliderFloat4(fieldName, ref newValue, range.Min, range.Max))
                {
                    ReflectField.Set(field, component, newValue);
                }
                else if (ImGui.DragFloat4(fieldName, ref newValue))
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
            else if (field.FieldType == typeof(GameObject))
            {
                ImGuiEx.RenderGameObjectSelect(fieldValue as GameObject, field.Name, (selectedGameObject) =>
                {
                    ReflectField.Set(field, component, selectedGameObject);
                });
            }
            else if (field.FieldType.IsSubclassOf(typeof(Asset)))
            {
                ImGuiEx.RenderAssetSelect(field.FieldType, fieldValue as Asset, fieldName, (selectedAsset) =>
                {
                    ReflectField.Set(field, component, selectedAsset);
                });
            }
            else
            {
                ImGui.Text($"{field.Name}: Unsupported type ({field.FieldType.Name})");
            }
        }

        private static bool HasAttribute<T>(this FieldInfo field, out T? attribute) where T : Attribute
        {
            attribute = field.GetCustomAttribute<T>();
            return attribute != null;
        }
    }
}

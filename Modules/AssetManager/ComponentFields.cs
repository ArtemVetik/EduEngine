using System.Reflection;

namespace EduEngine
{
    public static class ComponentFields
    {
        public static List<FieldInfo> FindAll(Component component)
        {
            var fields = new List<FieldInfo>();
            var baseType = component.GetType();

            while (baseType != null && baseType != typeof(Component))
            {
                var publicFields = baseType.GetFields(BindingFlags.Public | BindingFlags.Instance);
                var attributeFields = baseType.GetFields(BindingFlags.NonPublic | BindingFlags.Instance)
                    .Where(f => f.GetCustomAttribute<SerializeFieldAttribute>() != null);

                fields.AddRange(publicFields.Concat(attributeFields));
                baseType = baseType.BaseType;
            }

            return fields;
        }

        public static FieldInfo FindField(Type classType, string fieldName)
        {
            var field = classType.GetField(fieldName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
            var baseType = classType.BaseType;

            while (field == null && baseType != null && baseType != typeof(Component))
            {
                field = baseType.GetField(fieldName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                baseType = baseType.BaseType;
            }

            return field;
        }
    }
}
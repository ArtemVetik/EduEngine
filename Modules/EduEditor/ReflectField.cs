using System.Reflection;

namespace EduEngine.Editor
{
    internal static class ReflectField
    {
        public static void Set(FieldInfo? field, object? obj, object? value)
        {
            lock (GameObject.Lock)
            {
                if (field == null)
                    return;

                field.SetValue(obj, value);

                var flags = BindingFlags.NonPublic | BindingFlags.Public | BindingFlags.Instance;
                field.DeclaringType?.GetMethod("OnFieldChangedByReflection", flags)?.Invoke(obj, new object[] { field.Name });
            }
        }
    }
}

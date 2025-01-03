namespace EduEngine
{
    [AttributeUsage(AttributeTargets.Field)]
    public class SerializeFieldAttribute : Attribute
    {
    }

    [AttributeUsage(AttributeTargets.Field)]
    public class ColorAttribute : Attribute
    {
    }

    [AttributeUsage(AttributeTargets.Field)]
    public class RangeAttribute : Attribute
    {
        public readonly float Min;
        public readonly float Max;

        public RangeAttribute(float min, float max)
        {
            Min = min;
            Max = max;
        }
    }

    [AttributeUsage(AttributeTargets.Field)]
    public class MinAttribute : Attribute
    {
        public readonly float Value;

        public MinAttribute(float value)
        {
            Value = value;
        }
    }
}

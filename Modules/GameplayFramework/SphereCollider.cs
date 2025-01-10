using System.Diagnostics.CodeAnalysis;

namespace EduEngine
{
    public class SphereCollider : Collider
    {
        [SerializeField] private float _radius = 1.0f;

        public SphereCollider(GameObject parent)
            : base(parent, new SphereColliderData(1))
        { }

        public override void OnAddComponent()
        {
            Setup(_radius, GetMaxGlobalScale());
            base.OnAddComponent();
        }

        public override void OnCreate()
        {
            Setup(_radius, GetMaxGlobalScale());
            base.OnCreate();
        }

        public override void OnScaleChanged()
        {
            Setup(_radius, GetMaxGlobalScale());
        }

        public void Setup(float radius, float scale)
        {
            _radius = radius;

            var colliderData = new SphereColliderData(_radius * scale);
            SetShape(colliderData);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            OnScaleChanged();
        }

        private float GetMaxGlobalScale()
        {
            var scale = GameObject.Transform.LossyScale;
            return MathF.Max(MathF.Abs(scale.X), MathF.Max(MathF.Abs(scale.Y), MathF.Abs(scale.Z)));
        }
    }
}

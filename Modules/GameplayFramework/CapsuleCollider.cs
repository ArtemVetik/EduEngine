using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public class CapsuleCollider : Collider
    {
        [SerializeField] private float _radius = 0.5f;
        [SerializeField] private float _height = 0.5f;

        public CapsuleCollider(GameObject parent)
            : base(parent, new CapsuleColliderData(0.5f, 0.5f))
        { }

        public override void OnAddComponent()
        {
            Setup(_radius, _height, GameObject.Transform.LossyScale);
            base.OnAddComponent();
        }

        public override void OnCreate()
        {
            Setup(_radius, _height, GameObject.Transform.LossyScale);
            base.OnCreate();
        }

        public override void OnScaleChanged()
        {
            Setup(_radius, _height, GameObject.Transform.LossyScale);
        }

        public void Setup(float radius, float height, Vector3 scale)
        {
            _radius = radius;
            _height = height;

            var colliderData = new CapsuleColliderData(_radius * MathF.Max(scale.Y, scale.Z), _height * scale.X);
            SetShape(colliderData);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            OnScaleChanged();
        }
    }
}

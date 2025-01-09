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
            Setup(_radius);
            base.OnAddComponent();
        }

        public override void OnCreate()
        {
            Setup(_radius);
        }

        public void Setup(float radius)
        {
            _radius = radius;

            var colliderData = new SphereColliderData(_radius);
            SetShape(colliderData);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            Setup(_radius);
        }
    }
}

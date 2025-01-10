using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public class BoxCollider : Collider
    {
        [SerializeField] private float _width = 1;
        [SerializeField] private float _height = 1;
        [SerializeField] private float _depth = 1;

        public BoxCollider(GameObject parent)
            : base(parent, new BoxColliderData(1, 1, 1))
        { }

        public override void OnAddComponent()
        {
            Setup(_width, _height, _depth, GameObject.Transform.LossyScale);
            base.OnAddComponent();
        }

        public override void OnCreate()
        {
            Setup(_width, _height, _depth, GameObject.Transform.LossyScale);
            base.OnCreate();
        }

        public override void OnScaleChanged()
        {
            Setup(_width, _height, _depth, GameObject.Transform.LossyScale);
        }

        public void Setup(float width, float height, float depth, Vector3 scale)
        {
            _width = width;
            _height = height;
            _depth = depth;

            var colliderData = new BoxColliderData(_width * scale.X / 2, _height * scale.Y / 2, _depth * scale.Z / 2);
            SetShape(colliderData);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            OnScaleChanged();
        }
    }
}

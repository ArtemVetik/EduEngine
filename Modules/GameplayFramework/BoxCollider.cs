using System.Diagnostics.CodeAnalysis;

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
            Setup(_width, _height, _depth);
            base.OnAddComponent();
        }

        public override void OnCreate()
        {
            Setup(_width, _height, _depth);
            base.OnCreate();
        }

        public void Setup(float width, float height, float depth)
        {
            _width = width;
            _height = height;
            _depth = depth;

            var colliderData = new BoxColliderData(_width, _height, _depth);
            SetShape(colliderData);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            Setup(_width, _height, _depth);
        }
    }
}

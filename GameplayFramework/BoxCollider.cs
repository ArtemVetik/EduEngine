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

        public override void OnDeserialized()
        {
            Setup(_width, _height, _depth);
        }

        public override void OnCreate()
        {
            Setup(_width, _height, _depth);
        }

        public void Setup(float width, float height, float depth)
        {
            _width = width;
            _height = height;
            _depth = depth;

            var colliderData = new BoxColliderData(_width, _height, _depth);
            SetShape(colliderData);
        }
    }
}

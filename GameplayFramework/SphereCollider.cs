namespace EduEngine
{
    public class SphereCollider : Collider
    {
        [SerializeField] private float _radius = 1.0f;

        public SphereCollider(GameObject parent)
            : base(parent, new SphereColliderData(1))
        { }

        public override void OnDeserialized()
        {
            Setup(_radius);
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
    }
}

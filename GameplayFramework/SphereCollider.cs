namespace EduEngine
{
    public class SphereCollider : Collider
    {
        private SphereColliderData _colliderData;

        public SphereCollider(GameObject parent)
            : base(parent, new SphereColliderData(1))
        {
            _colliderData = new SphereColliderData(1);
        }

        public void Setup(float radius)
        {
            _colliderData = new SphereColliderData(radius);
            SetShape(_colliderData);
        }
    }
}

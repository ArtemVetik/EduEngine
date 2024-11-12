namespace EduEngine
{
    public class BoxCollider : Collider
    {
        private BoxColliderData _colliderData;

        public BoxCollider(GameObject parent)
            : base(parent, new BoxColliderData(1, 1, 1))
        {
            _colliderData = new BoxColliderData(1, 1, 1);
        }

        public void Setup(float width, float height, float depth)
        {
            _colliderData = new BoxColliderData(width, height, depth);
            SetShape(_colliderData);
        }
    }
}

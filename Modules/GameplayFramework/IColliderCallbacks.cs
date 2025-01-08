namespace EduEngine
{
    public interface IColliderCallbacks
    {
        void OnTriggerEnter(Collider other);
        void OnTriggerExit(Collider other);
        void OnCollisionEnter(CollisionData collisionData);
        void OnCollisionExit(CollisionData collisionData);
    }
}

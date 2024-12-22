namespace EduEngine
{
    public interface IColliderCallbacks
    {
        void OnTriggerEnter(Collider other);
        void OnTriggerExit(Collider other);
    }
}

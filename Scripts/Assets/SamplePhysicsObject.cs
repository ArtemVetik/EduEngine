namespace EduEngine.Scripts
{
    public class SamplePhysicsObject : Component, IColliderCallbacks
    {
        public SamplePhysicsObject(GameObject parent)
            : base(parent)
        {
        }

        public void OnTriggerEnter(Collider other)
        {
            Console.WriteLine($"Trigger Enter: {other.GameObject.Name}");
        }

        public void OnTriggerExit(Collider other)
        {
            Console.WriteLine($"Trigger Exit: {other.GameObject.Name}");
        }
    }
}

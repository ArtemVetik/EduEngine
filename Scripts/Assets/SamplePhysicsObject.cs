using System.Reflection;

namespace EduEngine.Scripts
{
    public class SamplePhysicsObject : Component, IColliderCallbacks
    {
        public SamplePhysicsObject(GameObject parent)
            : base(parent)
        {
        }

        public void OnCollisionEnter(CollisionData collisionData)
        {
            Console.WriteLine($"Collision Enter: {collisionData.Other}");
        }

        public void OnCollisionExit(CollisionData collisionData)
        {
            Console.WriteLine($"Collision Exit: {collisionData.Other}");
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

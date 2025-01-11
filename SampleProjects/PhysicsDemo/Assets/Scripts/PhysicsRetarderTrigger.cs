using EduEngine;
using System.Numerics;

namespace PhysicsDemo
{
    internal class PhysicsRetarderTrigger : Component, IColliderCallbacks
    {
        [SerializeField, Range(0.0f, 1.0f)] private float _reducePower = 0.9f;
        [SerializeField] private GameObject _checkObject;

        private float _checkTime = 0.0f;

        public PhysicsRetarderTrigger(GameObject parent) : base(parent)
        {
        }

        public override void OnGameStart()
        {
            _checkObject.Transform.LocalScale = Vector3.Zero;
        }

        public override void Update()
        {
            if (_checkTime < 0.0f)
                return;

            _checkTime -= EduTime.DeltaTime;

            if (_checkTime < 0.0f)
                _checkObject.Transform.LocalScale = Vector3.Zero;
        }

        public void OnCollisionEnter(CollisionData collisionData)
        {

        }

        public void OnCollisionExit(CollisionData collisionData)
        {

        }

        public void OnTriggerEnter(Collider other)
        {
            if (other.ParentBody != null)
            {
                other.ParentBody.AddForce(-other.ParentBody.Velocity * _reducePower, ForceMode.VELOCITY_CHANGE);

                _checkObject.Transform.LocalScale = Vector3.One;
                _checkTime = 1.0f;
            }
        }

        public void OnTriggerExit(Collider other)
        {
            
        }
    }
}

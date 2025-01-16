using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    public class FallingPlatform : Component, IColliderCallbacks
    {
        [SerializeField] private float _fallSpeed = 1.0f;

        private RigidBody _body;
        private bool _falling = false;

        public FallingPlatform(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
        }

        public override void Update()
        {
            if (_falling == false)
                return;

            _body.SetTransform(GameObject.Transform.Position -Vector3.UnitY * _fallSpeed * EduTime.DeltaTime, GameObject.Transform.Rotation);
        }

        public void OnCollisionEnter(CollisionData collisionData)
        {
            _falling = true;
        }

        public void OnCollisionExit(CollisionData collisionData) { }
        public void OnTriggerEnter(Collider other) { }
        public void OnTriggerExit(Collider other) { }
    }
}

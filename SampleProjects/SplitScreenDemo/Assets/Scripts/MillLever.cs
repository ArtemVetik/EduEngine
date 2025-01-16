using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    public class MillLever : Component
    {
        [SerializeField] private GameObject _platform;
        [SerializeField] private float _moveSpeed = 0.1f;

        private RigidBody _body;
        private RigidBody _platformBody;

        public MillLever(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
            _platformBody = _platform.GetComponent<RigidBody>();
        }

        public override void PhysicsUpdate()
        {
            _body.AngularVelocity = Vector3.Lerp(_body.AngularVelocity, Vector3.Zero, 25.0f * EduTime.DeltaTime);

            _platform.Transform.Position += Vector3.UnitX * _body.AngularVelocity.Y * _moveSpeed;
            _platformBody.SetTransform(_platform.Transform.Position, _platform.Transform.Rotation);
        }
    }
}

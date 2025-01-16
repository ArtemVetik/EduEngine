using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    public class Jetpack : Component, IColliderCallbacks
    {
        private RigidBody _body;
        private PlayerMovement _target;
        private ParticleSystem _jetpackEffect;

        public Jetpack(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
            _jetpackEffect = GameObject.GetComponentsInChildren<ParticleSystem>().First();
        }

        public override void PhysicsUpdate()
        {
            if (_target != null)
                _body.SetTransform(_target.GameObject.Transform.Position, _target.GameObject.Transform.Rotation);
        }

        public void OnTriggerEnter(Collider other)
        {
            if (_target != null)
                return;

            var target = other.GameObject.GetComponentsInParent<PlayerMovement>().FirstOrDefault();

            if (target == null)
                return;

            if (target.HasJetpack)
                return;

            _target = target;
            _target.AddJetpack(this);
            GameObject.SetParent(_target.GameObject);
            GameObject.Transform.LocalPosition = Vector3.Zero;
        }

        public void OnCollisionEnter(CollisionData collisionData) { }
        public void OnCollisionExit(CollisionData collisionData) { }
        public void OnTriggerExit(Collider other) { }

        public void SetEffectActive(bool active) => _jetpackEffect.SetEmissionState(active);
    }
}

using EduEngine;
using System.Numerics;

namespace PhysicsDemo
{
    internal class Mill : Component
    {
        [SerializeField] private float _rotateSpeed = 1.0f;

        private RigidBody _body;

        public Mill(GameObject parent)
            : base(parent)
        {
        }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
        }

        public override void Update()
        {
            GameObject.Transform.Rotation = Quaternion.CreateFromYawPitchRoll(0, _rotateSpeed * EduTime.TotalTime, 0);
            _body.SetTransform(GameObject.Transform.Position, GameObject.Transform.Rotation);
        }
    }
}

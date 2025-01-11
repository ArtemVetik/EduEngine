using EduEngine;
using System.Numerics;

namespace PhysicsDemo
{
    internal class RotateAround : Component
    {
        [SerializeField] private float _xRotate = 0.0f;
        [SerializeField] private float _yRotate = 0.0f;
        [SerializeField] private float _zRotate = 0.0f;

        private RigidBody _body;

        public RotateAround(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
        }

        public override void Update()
        {
            GameObject.Transform.Rotation = Quaternion.CreateFromYawPitchRoll(_xRotate * EduTime.TotalTime,
                                                                                _yRotate * EduTime.TotalTime,
                                                                                _zRotate * EduTime.TotalTime);

            _body.SetTransform(GameObject.Transform.Position, GameObject.Transform.Rotation);
        }
    }
}

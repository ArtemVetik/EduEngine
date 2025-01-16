using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    public class RotateBridge : Component, ITriggerTarget
    {
        [SerializeField] private float _speed = 5.0f;

        private RigidBody _body;
        private Quaternion _targetRotation;

        public RotateBridge(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
            _targetRotation = GameObject.Transform.Rotation;
        }

        public override void Update()
        {
            GameObject.Transform.Rotation = Quaternion.Lerp(GameObject.Transform.Rotation, _targetRotation, _speed * EduTime.DeltaTime);
            _body.SetTransform(GameObject.Transform.Position, GameObject.Transform.Rotation);
        }

        public void StartAction()
        {
            _targetRotation = Quaternion.Identity;
        }

        public void EndAction()
        {
            _targetRotation = Quaternion.CreateFromYawPitchRoll(0, -MathF.PI / 2, 0);
        }
    }
}

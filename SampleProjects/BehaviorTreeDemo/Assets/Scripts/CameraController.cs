using EduEngine;
using System.Numerics;

namespace BehaviorTreeDemo
{
    public class CameraController : Component
    {
        [SerializeField] private GameObject _target;
        [SerializeField] private float _distance;

        private float _yAngle;
        private float _height = 0.5f;

        public CameraController(GameObject parent)
            : base(parent)
        { }

        public override void Update()
        {
            if (Input.Runtime.KeyPressed(KeyCode.LSHIFT) == false)
            {
                _yAngle -= Input.Runtime.MouseDelta.X * 0.01f;
                _height += Input.Runtime.MouseDelta.Y * 0.002f;

                _height = Math.Clamp(_height, 0.1f, 0.9f);
            }

            _distance -= Input.Runtime.ScrollDelta.Y * 0.01f;
            _distance = Math.Clamp(_distance, 2.0f, 100.0f);

            var direction = new Vector3(MathF.Cos(_yAngle), _height, MathF.Sin(_yAngle));

            GameObject.Transform.Position = _target.Transform.Position + direction * _distance;
            GameObject.Transform.Rotation = Extentions.LookRotation(-direction, Vector3.UnitY);
        }
    }
}

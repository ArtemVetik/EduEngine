using System.Numerics;

namespace EduEngine.Scripts
{
    public class TestCameraController : Component
    {
        [SerializeField] private float _moveSpeed = 20.0f;
        [SerializeField] private float _moveStrength = 8.0f;
        [SerializeField] private float _rotateStrength = 20.0f;
        
        private Vector3 _targetPosition;
        private Quaternion _targetRotation;

        public TestCameraController(GameObject parent)
            : base(parent)
        {
            _targetPosition = GameObject.Transform.LocalPosition;
            _targetRotation = GameObject.Transform.LocalRotation;
        }

        public override void Update()
        {
            var forward = GameObject.Transform.Forward;
            var right = GameObject.Transform.Right;
            var up = GameObject.Transform.Up;

            if (Input.Runtime.KeyPressed(KeyCode.W))
                _targetPosition += _moveSpeed * forward * EduTime.DeltaTime;
            if (Input.Runtime.KeyPressed(KeyCode.S))
                _targetPosition += _moveSpeed  * - forward * EduTime.DeltaTime;
            if (Input.Runtime.KeyPressed(KeyCode.D))
                _targetPosition += _moveSpeed * right * EduTime.DeltaTime;
            if (Input.Runtime.KeyPressed(KeyCode.A))
                _targetPosition += _moveSpeed * -right * EduTime.DeltaTime;
            if (Input.Runtime.KeyPressed(KeyCode.E))
                _targetPosition += _moveSpeed * up * EduTime.DeltaTime;
            if (Input.Runtime.KeyPressed(KeyCode.Q))
                _targetPosition += _moveSpeed  * - up * EduTime.DeltaTime;

            if (Input.Runtime.MouseButtonPressed(MouseCode.Mouse1))
            {
                var yaw = Quaternion.CreateFromAxisAngle(Vector3.UnitY, 0.01f * Input.Runtime.MouseDelta.X);
                var pitch = Quaternion.CreateFromAxisAngle(Vector3.Transform(Vector3.UnitX, _targetRotation), 0.01f * Input.Runtime.MouseDelta.Y);
                _targetRotation = Quaternion.Multiply(yaw, _targetRotation);
                _targetRotation = Quaternion.Multiply(pitch, _targetRotation);
            }

            GameObject.Transform.LocalPosition = Vector3.Lerp(GameObject.Transform.LocalPosition, _targetPosition, _moveStrength * EduTime.DeltaTime);
            GameObject.Transform.LocalRotation = Quaternion.Lerp(GameObject.Transform.LocalRotation, _targetRotation, _rotateStrength * EduTime.DeltaTime);
        }
    }
}

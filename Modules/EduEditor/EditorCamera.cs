using System.Numerics;

namespace EduEngine.Editor
{
    public class EditorCamera : IDisposable
    {
        private readonly float _moveSpeed = 20.0f;
        private readonly float _moveStrength = 8.0f;
        private readonly float _rotateStrength = 20.0f;

        private Vector3 _position;
        private Vector3 _targetPosition;
        private Quaternion _rotation;
        private Quaternion _targetRotation;
        private bool _moveActive = false;

        private NativeCameraWrapper _nativeCamera;

        public bool DebugDraw
        {
            get => _nativeCamera.IsDebugRenderEnabled();
            set => _nativeCamera.SetDebugRenderEnable(value);
        }
        public Vector3 Forward => Vector3.Transform(Vector3.UnitZ, _rotation);
        public Vector3 Right => Vector3.Transform(Vector3.UnitX, _rotation);
        public Vector3 Up => Vector3.Transform(Vector3.UnitY, _rotation);
        public Matrix4x4 ViewMatrix => _nativeCamera.GetView();
        public Matrix4x4 ProjectionMatrix => _nativeCamera.GetProjection();

        public EditorCamera()
        {
            _position = new Vector3(0, 5, -10);
            _targetPosition = _position;
            _rotation = Quaternion.Identity;
            _targetRotation = _rotation;

            _nativeCamera = new NativeCameraWrapper();
            _nativeCamera.SetDebugRenderEnable(true);
        }

        public void Dispose()
        {
            _nativeCamera.Dispose();
        }

        public void Update()
        {
            if (Input.Runtime.MouseButtonDown(MouseCode.Mouse1))
            {
                if (Input.Runtime.MousePosition.X >= 0 &&
                    Input.Runtime.MousePosition.Y >= 0 &&
                    Input.Runtime.MousePosition.X < Screen.Size.X &&
                    Input.Runtime.MousePosition.Y < Screen.Size.Y)
                    _moveActive = true;
            }
            if (Input.Runtime.MouseButtonUp(MouseCode.Mouse1))
            {
                _moveActive = false;
            }

            if (_moveActive)
            {
                var yaw = Quaternion.CreateFromAxisAngle(Vector3.UnitY, 0.01f * Input.Runtime.MouseDelta.X);
                var pitch = Quaternion.CreateFromAxisAngle(Vector3.Normalize(Vector3.Transform(Vector3.UnitX, _targetRotation)), 0.01f * Input.Runtime.MouseDelta.Y);

                _targetRotation = Quaternion.Multiply(pitch, _targetRotation);
                _targetRotation = Quaternion.Multiply(yaw, _targetRotation);

                if (Input.Runtime.KeyPressed(KeyCode.W))
                    _targetPosition += _moveSpeed * Forward * EduTime.DeltaTime;
                if (Input.Runtime.KeyPressed(KeyCode.S))
                    _targetPosition += _moveSpeed * -Forward * EduTime.DeltaTime;
                if (Input.Runtime.KeyPressed(KeyCode.D))
                    _targetPosition += _moveSpeed * Right * EduTime.DeltaTime;
                if (Input.Runtime.KeyPressed(KeyCode.A))
                    _targetPosition += _moveSpeed * -Right * EduTime.DeltaTime;
                if (Input.Runtime.KeyPressed(KeyCode.E))
                    _targetPosition += _moveSpeed * Up * EduTime.DeltaTime;
                if (Input.Runtime.KeyPressed(KeyCode.Q))
                    _targetPosition += _moveSpeed * -Up * EduTime.DeltaTime;
            }

            _position = Vector3.Lerp(_position, _targetPosition, _moveStrength * EduTime.DeltaTime);
            _rotation = Quaternion.Lerp(_rotation, _targetRotation, _rotateStrength * EduTime.DeltaTime);

            _nativeCamera?.Update(Forward, Right, Up, _position);
        }

        public void Render()
        {
            lock (GameObject.Lock)
            {
                _nativeCamera.Render();
            }
        }
    }
}

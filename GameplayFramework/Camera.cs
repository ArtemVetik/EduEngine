using System.Numerics;

namespace EduEngine
{
    public class Camera : Component, IDisposable
    {
        private NativeCameraWrapper _camera;

        public Camera(GameObject parent)
            : base(parent)
        {
            _camera = new NativeCameraWrapper();
        }

        public override void Update()
        {
            var forward = GameObject.Transform.Forward;
            var right = GameObject.Transform.Right;
            var up = GameObject.Transform.Up;

            var moveSpeed = 50.0f;

            if (Input.KeyPressed(KeyCode.W))
                GameObject.Transform.Position += moveSpeed * forward * EduTime.DeltaTime;
            if (Input.KeyPressed(KeyCode.S))
                GameObject.Transform.Position += moveSpeed * -forward * EduTime.DeltaTime;
            if (Input.KeyPressed(KeyCode.D))
                GameObject.Transform.Position += moveSpeed * right * EduTime.DeltaTime;
            if (Input.KeyPressed(KeyCode.A))
                GameObject.Transform.Position += moveSpeed * -right * EduTime.DeltaTime;
            if (Input.KeyPressed(KeyCode.E))
                GameObject.Transform.Position += moveSpeed * up * EduTime.DeltaTime;
            if (Input.KeyPressed(KeyCode.Q))
                GameObject.Transform.Position += moveSpeed * -up * EduTime.DeltaTime;

            if (Input.MouseButtonPressed(MouseCode.Mouse1))
            {
                var yaw = Quaternion.CreateFromAxisAngle(Vector3.UnitY, 0.01f * Input.MouseDelta.X);
                var pitch = Quaternion.CreateFromAxisAngle(GameObject.Transform.Right, 0.01f * Input.MouseDelta.Y);
                GameObject.Transform.Rotation = Quaternion.Multiply(yaw, GameObject.Transform.Rotation);
                GameObject.Transform.Rotation = Quaternion.Multiply(pitch, GameObject.Transform.Rotation);
            }

            _camera?.Update(GameObject.Transform.Forward, GameObject.Transform.Right, GameObject.Transform.Up, GameObject.Transform.Position);
        }

        public void Dispose()
        {
            _camera.Dispose();
            _camera = null;
        }
    }
}

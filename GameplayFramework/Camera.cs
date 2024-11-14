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

            if (InputSystemInterop.IsKeyPressed(0x11)) // W
                GameObject.Transform.Position += moveSpeed * forward * EduTime.DeltaTime;
            if (InputSystemInterop.IsKeyPressed(0x1F)) // S
                GameObject.Transform.Position += moveSpeed * -forward * EduTime.DeltaTime;
            if (InputSystemInterop.IsKeyPressed(0x20)) // D
                GameObject.Transform.Position += moveSpeed * right * EduTime.DeltaTime;
            if (InputSystemInterop.IsKeyPressed(0x1E)) // A
                GameObject.Transform.Position += moveSpeed * -right * EduTime.DeltaTime;
            if (InputSystemInterop.IsKeyPressed(0x12)) // E
                GameObject.Transform.Position += moveSpeed * up * EduTime.DeltaTime;
            if (InputSystemInterop.IsKeyPressed(0x10)) // Q
                GameObject.Transform.Position += moveSpeed * -up * EduTime.DeltaTime;

            var mouseState = InputSystemInterop.GetMouseState();

            if ((mouseState.rgbButtons[1] & 0x80) != 0)
            {
                var yaw = Quaternion.CreateFromAxisAngle(Vector3.UnitY, 0.01f * mouseState.lX);
                var pitch = Quaternion.CreateFromAxisAngle(GameObject.Transform.Right, 0.01f * mouseState.lY);
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

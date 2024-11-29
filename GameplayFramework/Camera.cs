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

        public Matrix4x4 ViewMatrix => _camera.GetView();
        public Matrix4x4 ProjectionMatrix => _camera.GetProjection();

        public override void Update()
        {
            _camera?.Update(GameObject.Transform.Forward, GameObject.Transform.Right, GameObject.Transform.Up, GameObject.Transform.Position);
        }

        public override void OnRender()
        {
            _camera.Render();
        }

        public void Dispose()
        {
            _camera.Dispose();
            _camera = null;
        }
    }
}

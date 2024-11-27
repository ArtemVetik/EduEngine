
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

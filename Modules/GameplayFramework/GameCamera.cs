using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public class GameCamera : Component, IDisposable
    {
        [SerializeField] private float _fov = 55.0f;
        [SerializeField] private float _near = 0.3f;
        [SerializeField] private float _far = 1000.0f;
        [SerializeField, Color] private Vector4 _backgroundColor = new Vector4(0, 0, 0, 1);
        [SerializeField, Range(0.0f, 1.0f)] private Vector4 _viewport = new Vector4(0, 0, 1, 1);
        [SerializeField] private bool _debugDraw = false;

        private NativeCameraWrapper _camera;

        public GameCamera(GameObject parent)
            : base(parent)
        {
            _camera = new NativeCameraWrapper();
        }

        public Matrix4x4 ViewMatrix => _camera.GetView();
        public Matrix4x4 ProjectionMatrix => _camera.GetProjection();

        public override void OnAddComponent()
        {
            _camera.SetProjectionMatrix(_fov, _near, _far);
            _camera.SetViewport(_viewport);
            _camera.SetBackgroundColor(_backgroundColor);
        }

        public override void OnCreate()
        {
            OnAddComponent();
        }

        public override void Update()
        {
            _camera?.Update(GameObject.Transform.Forward, GameObject.Transform.Right, GameObject.Transform.Up, GameObject.Transform.Position);
        }

        public override void UpdateEditor()
        {
            _camera?.Update(GameObject.Transform.Forward, GameObject.Transform.Right, GameObject.Transform.Up, GameObject.Transform.Position);
            _camera?.DrawFrustrum();
        }

        public override void OnRender()
        {
            _camera?.Render();

            if (_debugDraw)
                _camera?.DebugRender();
        }

        public void Dispose()
        {
            _camera.Dispose();
            _camera = null;
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            OnAddComponent();
        }
    }
}

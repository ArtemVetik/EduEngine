namespace EduEngine
{
    public class Renderer : Component, IDisposable
    {
        private NativeRenderObjectWrapper _renderObject;

        public Renderer(GameObject parent)
            : base(parent)
        {
            _renderObject = null;
        }

        public override void Update()
        {
            _renderObject?.SetWorldMatrix(GameObject.Transform.WorldMatrix);
        }

        public void SetMesh(MeshData mesh)
        {
            _renderObject?.Dispose();
            _renderObject = new NativeRenderObjectWrapper(mesh);
        }

        public void Dispose()
        {
            _renderObject?.Dispose();
            _renderObject = null;
        }
    }
}

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
            _renderObject?.SetWorldMatrix(Parent.Transform.WorldMatrix);
        }

        public void SetMesh(MeshData mesh)
        {
            if (_renderObject != null)
                RenderEngineInterop.RemoveObject(_renderObject);
            
            _renderObject = RenderEngineInterop.AddObject(mesh);
        }

        public void Dispose()
        {
            _renderObject?.Dispose();
            _renderObject = null;
        }
    }
}

namespace EduEngine
{
    public class Renderer : Component, IDisposable, ISerializeCallback
    {
        [SerializeField] private SharedMesh _mesh;

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

        public override void UpdateEditor()
        {
            _renderObject?.SetWorldMatrix(GameObject.Transform.WorldMatrix);
        }

        public void SetMesh(SharedMesh mesh)
        {
            _mesh = mesh;
            _renderObject?.Dispose();

            if (_mesh != null)
                _renderObject = new NativeRenderObjectWrapper(_mesh.GetWrapper());
            else
                _renderObject = null;
        }

        public void Dispose()
        {
            _renderObject?.Dispose();
            _renderObject = null;
        }

        public override void OnAddComponent()
        {
            if (_mesh != null)
                SetMesh(_mesh);
        }

        void ISerializeCallback.OnDeserialize()
        {
            SetMesh(_mesh);
        }
    }
}

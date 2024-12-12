namespace EduEngine
{
    public class Renderer : Component, IDisposable, ISerializeCallback
    {
        [SerializeField] private SharedMesh _mesh;
        [SerializeField] private Material _material;

        private NativeRenderObjectWrapper _renderObject;

        public Renderer(GameObject parent)
            : base(parent)
        {
            _renderObject = new NativeRenderObjectWrapper();
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

            if (mesh == null)
                _renderObject?.SetMesh(null);
            else
                _renderObject?.SetMesh(_mesh.GetWrapper());
        }

        public void SetMaterial(Material material)
        {
            _material = material;

            if (material == null)
                _renderObject?.SetMaterial(null);
            else
                _renderObject?.SetMaterial(_material.Wrapper);
        }

        public void Dispose()
        {
            _renderObject?.Dispose();
            _renderObject = null;
        }

        public override void OnAddComponent()
        {
            SetMesh(_mesh);
            SetMaterial(_material);
        }

        void ISerializeCallback.OnDeserialize()
        {
            SetMesh(_mesh);
            SetMaterial(_material);
        }
    }
}

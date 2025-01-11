using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public class Renderer : Component, IDisposable
    {
        [SerializeField] private SharedMesh _mesh;
        [SerializeField] private Material _material;
        [SerializeField] private Vector2 _texScale = Vector2.One;

        private NativeRenderObjectWrapper _renderObject;

        public Renderer(GameObject parent)
            : base(parent)
        {
            _renderObject = new NativeRenderObjectWrapper();
        }

        public Material Material => _material;
        public Vector2 TextureScale => _texScale;

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

        public void SetTextureScale(Vector2 newTexScale)
        {
            _texScale = newTexScale;
            _renderObject?.SetTextureTransform(Matrix4x4.CreateScale(new Vector3(_texScale.X, _texScale.Y, 0)));
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
            _renderObject?.SetTextureTransform(Matrix4x4.CreateScale(new Vector3(_texScale.X, _texScale.Y, 0)));
        }

        internal NativeRenderObjectWrapper GetWrapper() => _renderObject;

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            OnAddComponent();
        }
    }
}

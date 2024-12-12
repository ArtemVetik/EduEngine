namespace EduEngine
{
    public class Material : Asset
    {
        private MaterialWrapper _nativeMaterial;
        private Texture _mainTexture;

        public Material(string guid) : base(guid)
        {
            _nativeMaterial = new MaterialWrapper();
        }

        public Texture MainTexture => _mainTexture;
        internal MaterialWrapper Wrapper => _nativeMaterial;

        public void SetMainTexture(Texture texture)
        {
            if (_nativeMaterial == null)
                return;

            _mainTexture = texture;

            if (texture == null)
                _nativeMaterial.SetTexture(null);
            else
                _nativeMaterial.SetTexture(texture.GetWrapper());
        }

        protected override void OnDelete()
        {
            _nativeMaterial.Dispose();
            _nativeMaterial = null;
        }

        protected override void OnPathUpdated(string globalPath) { }
    }
}

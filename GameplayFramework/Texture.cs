namespace EduEngine
{
    public class Texture : Asset
    {
        private TextureWrapper _nativeTextureWrapper;

        internal Texture(string guid, string assetPath) : base(guid)
        {
            _nativeTextureWrapper = new TextureWrapper(assetPath);
        }

        protected override void OnDelete()
        {
            _nativeTextureWrapper.Dispose();
        }

        internal TextureWrapper GetWrapper() => _nativeTextureWrapper;

        protected override void OnPathUpdated(string globalPath)
        {
            _nativeTextureWrapper?.UpdateFilePath(globalPath);
        }
    }
}

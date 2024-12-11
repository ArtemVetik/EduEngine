namespace EduEngine.Editor
{
    internal static class MeshPreview
    {
        private static AssetData? _assetData = null;

        public static nint TexturePtr { get; private set; } = nint.Zero;

        public static void UpdatePreview(string guid)
        {
            var textureAsset = AssetDataBase.HasGUID(guid)
                ? AssetDataBase.AllAssets[guid]
                : null;

            if (textureAsset == _assetData)
                return;

            _assetData = textureAsset;

            if (_assetData == null || _assetData.Asset == null || _assetData.Type != AssetType.Mesh)
            {
                EditorRenderEngineInterop.SetPreviewMesh(null);
                TexturePtr = nint.Zero;
            }
            else
                TexturePtr = EditorRenderEngineInterop.SetPreviewMesh(_assetData.GlobalPath);
        }
    }
}

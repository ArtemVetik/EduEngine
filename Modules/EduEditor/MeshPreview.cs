namespace EduEngine.Editor
{
    internal static class MeshPreview
    {
        private static AssetData? _assetData = null;

        public static PreviewMeshInfo Info { get; private set; } = new PreviewMeshInfo();

        public static void UpdatePreview(string guid)
        {
            var textureAsset = AssetDataBase.HasGUID(guid)
                ? AssetDataBase.AllAssets[guid]
                : null;

            if (textureAsset == _assetData)
                return;

            _assetData = textureAsset;

            if (_assetData == null || _assetData.Asset == null || _assetData.Type != AssetType.Mesh)
                Info = EditorRenderEngineInterop.SetPreviewMesh(null);
            else
                Info = EditorRenderEngineInterop.SetPreviewMesh(_assetData.GlobalPath);
        }
    }
}

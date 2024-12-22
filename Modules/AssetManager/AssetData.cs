namespace EduEngine
{
    public class AssetData
    {
        public readonly AssetMetaData MetaData;

        private readonly string _rootPath;

        internal AssetData(string rootPath, string localPath, AssetMetaData metaData)
        {
            _rootPath = rootPath;
            LocalPath = localPath;
            MetaData = metaData;
        }

        public string GlobalPath => _rootPath + LocalPath;
        public AssetType Type => Path.GetExtension(LocalPath).ToAssetType();
        public string LocalPath { get; internal set; }
        public Asset? Asset { get; internal set; }
    }
}

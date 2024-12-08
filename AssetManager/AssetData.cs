namespace EduEngine
{
    public class AssetData
    {
        private readonly string _rootPath;
        public readonly string LocalPath;
        public readonly AssetMetaData MetaData;

        internal AssetData(string rootPath, string localPath, AssetMetaData metaData)
        {
            _rootPath = rootPath;
            LocalPath = localPath;
            MetaData = metaData;
        }

        public string GlobalPath => _rootPath + LocalPath;
        public Asset? Asset { get; internal set; }

        public AssetType Type
        {
            get
            {
                var extention = Path.GetExtension(LocalPath);

                switch (extention)
                {
                    case ".scene": return AssetType.Scene;
                    case ".fbx": return AssetType.Mesh;
                    case ".cs": return AssetType.Script;
                    default: return AssetType.Invalid;
                }
            }
        }
    }
}

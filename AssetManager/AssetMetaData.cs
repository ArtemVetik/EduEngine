namespace EduEngine
{
    internal class AssetMetaData
    {
        public string GUID { get; set; }
        public bool IsFolder { get; set; }

        internal static AssetMetaData NewData(bool isFolder)
        {
            return new AssetMetaData()
            {
                GUID = Guid.NewGuid().ToString(),
                IsFolder = isFolder
            };
        }
    }
}

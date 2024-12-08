using Newtonsoft.Json;

namespace EduEngine
{
    public static class AssetDataBase
    {
        private static AssetResolver _assetResolver = null;
        private static Dictionary<string, AssetData> _assets = new();

        public static bool IsDirty => _assetResolver.HasChanges;
        public static string RootPath => _assetResolver.RootPath;

        public static Dictionary<string, AssetData> AllAssets => _assets;

        public static void Initialize(string rootPath)
        {
            _assetResolver = new AssetResolver(rootPath);
            Resolve();
        }

        public static void Dispose()
        {
            _assetResolver.Dispose();
        }

        public static bool HasGUID(string guid) => _assets.ContainsKey(guid);

        public static AssetData GetAssetData(string guid)
        {
            return _assets[guid];
        }

        public static string GetGUIDByLocalPath(string localPath)
        {
            return _assets.FirstOrDefault(asset => asset.Value.LocalPath == localPath).Key;
        }

        public static void Resolve()
        {
            var deleteId = new List<string>();
            var assets = _assetResolver.Resolve();

            foreach (var item in _assets)
            {
                if (assets.ContainsKey(item.Key))
                {
                    assets.Remove(item.Key);
                }
                else
                {
                    deleteId.Add(item.Key);
                }
            }

            foreach (var guid in deleteId)
            {
                _assets[guid].Asset?.Delete();
                _assets.Remove(guid);
            }

            foreach (var item in assets)
            {
                var metadataPath = RootPath + item.Value + ".meta";
                var metaData = JsonConvert.DeserializeObject<AssetMetaData>(File.ReadAllText(metadataPath));

                _assets.Add(item.Key, new AssetData(RootPath, item.Value, metaData));
                _assets[item.Key].Asset = CreateAsset(item.Key);
            }
        }

        private static Asset CreateAsset(string guid)
        {
            Asset asset = null;
            var assetData = GetAssetData(guid);

            switch (assetData.Type)
            {
                case AssetType.Mesh:
                    asset = new SharedMesh(guid, assetData.GlobalPath);
                    break;
            }

            return asset;
        }
    }
}

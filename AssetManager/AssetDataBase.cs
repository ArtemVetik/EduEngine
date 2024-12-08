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

        public static bool CreateScene(string assetPath, SceneData scene)
        {
            if (_assetResolver == null)
                return false;

            var fullPath = $"{_assetResolver.RootPath}{assetPath}.scene";

            if (_assetResolver.IsPathInside(fullPath) == false)
                return false;

            File.WriteAllText(fullPath, JsonConvert.SerializeObject(scene, Formatting.Indented));
            Resolve();
            return true;
        }

        public static bool LoadScene(string assetPath, out SceneData? scene)
        {
            scene = null;

            if (_assetResolver == null)
                return false;

            if (Path.GetExtension(assetPath) == ".scene")
                assetPath = Path.Combine(
                   Path.GetDirectoryName(assetPath) ?? string.Empty,
                   Path.GetFileNameWithoutExtension(assetPath)
                );

            var fullPath = $"{_assetResolver.RootPath}{assetPath}.scene";

            if (_assetResolver.IsPathInside(fullPath) == false)
                return false;

            if (File.Exists(fullPath) == false)
                return false;

            scene = JsonConvert.DeserializeObject<SceneData>(File.ReadAllText(fullPath));
            return scene != null;
        }

        public static string GetScriptGUID(Type scriptType)
        {
            var scripts = GetByType(AssetType.Script);

            foreach (var script in scripts)
            {
                var type = ScriptParser.FindComponent(_assetResolver.RootPath + script.Value.LocalPath);

                if (type == scriptType)
                    return script.Key;
            }

            return null;
        }

        public static bool HasGUID(string guid) => _assets.ContainsKey(guid);

        public static string GetLocalPathByGUID(string guid)
        {
            return _assets[guid].LocalPath;
        }

        public static AssetData GetByGUID(string guid)
        {
            return _assets[guid];
        }

        public static string GetGlobalPathByGUID(string guid)
        {
            return _assetResolver.RootPath + _assets[guid].LocalPath;
        }

        public static string GetGUIDByLocalPath(string localPath)
        {
            return _assets.FirstOrDefault(asset => asset.Value.LocalPath == localPath).Key;
        }

        public static string GetGUIDByGlobalPath(string globalPath)
        {
            var localPath = globalPath.Remove(0, _assetResolver.RootPath.Length);
            return GetGUIDByLocalPath(localPath);
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
                _assets.Add(item.Key, new AssetData() { LocalPath = item.Value });
                _assets[item.Key] = CreateAssetData(item.Key);
            }
        }

        public static IEnumerable<KeyValuePair<string, AssetData>> GetByType(AssetType type)
        {
            switch (type)
            {
                case AssetType.Scene:
                    return _assets.Where(data => Path.GetExtension(data.Value.LocalPath) == ".scene");
                case AssetType.Script:
                    return _assets.Where(data => Path.GetExtension(data.Value.LocalPath) == ".cs");
                case AssetType.Mesh:
                    return _assets.Where(data => Path.GetExtension(data.Value.LocalPath) == ".fbx");
                default:
                    return null;
            }
        }

        public static AssetType GetType(string guid)
        {
            _assets.TryGetValue(guid, out AssetData assetPath);

            if (assetPath == null)
                return AssetType.Invalid;

            var extention = Path.GetExtension(assetPath.LocalPath);

            switch (extention)
            {
                case ".scene": return AssetType.Scene;
                case ".fbx": return AssetType.Mesh;
                case ".cs": return AssetType.Script;
                default: return AssetType.Invalid;
            }
        }

        public static AssetMetaData GetMetaData(string guid)
        {
            var assetPath = GetGlobalPathByGUID(guid) + ".meta";
            return JsonConvert.DeserializeObject<AssetMetaData>(File.ReadAllText(assetPath));
        }

        public static AssetData[] GetByAssetType(Type type)
        {
            var assets = new List<AssetData>();

            foreach (var item in _assets)
            {
                if (item.Value.Asset != null && item.Value.Asset.GetType() == type)
                    assets.Add(item.Value);
            }

            return assets.ToArray();
        }

        private static AssetData CreateAssetData(string guid)
        {
            var type = GetType(guid);
            var globalPath = GetGlobalPathByGUID(guid);
            var localPath = GetLocalPathByGUID(guid);
            Asset asset = null;

            switch (type)
            {
                case AssetType.Mesh:
                    asset = new SharedMesh(guid, globalPath);
                    break;
            }

            return new AssetData()
            {
                LocalPath = localPath,
                Asset = asset,
            };
        }
    }
}

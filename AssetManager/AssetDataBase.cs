using Newtonsoft.Json;

namespace EduEngine
{
    public static class AssetDataBase
    {
        private static AssetResolver _assetResolver = null;
        private static Dictionary<string, string> _assets = new();

        public static bool IsDirty => _assetResolver.HasChanges;

        public static Dictionary<string, string> AllAssets => _assets;

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

            if (_assetResolver.IsPathInside(assetPath) == false)
                return false;

            if (Directory.Exists(assetPath) == false)
                return false;

            File.WriteAllText(assetPath, JsonConvert.SerializeObject(scene));
            Resolve();
            return true;
        }

        public static bool LoadScene(string assetPath, out SceneData? scene)
        {
            scene = null;

            if (_assetResolver == null)
                return false;

            if (_assetResolver.IsPathInside(assetPath) == false)
                return false;

            if (File.Exists(assetPath) == false)
                return false;

            scene = JsonConvert.DeserializeObject<SceneData>(File.ReadAllText(assetPath));
            return scene != null;
        }

        public static string GetScriptGUID(Type scriptType)
        {
            var scripts = GetByType(AssetType.Script);

            foreach (var script in scripts)
            {
                if (Path.GetFileNameWithoutExtension(script.Value) == scriptType.Name)
                    return script.Key;
            }

            return null;
        }

        public static string GetPathByGUID(string guid)
        {
            return _assets[guid];
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
                _assets.Remove(guid);

            foreach (var item in assets)
                _assets.Add(item.Key, item.Value);
        }

        public static IEnumerable<KeyValuePair<string, string>> GetByType(AssetType type)
        {
            switch (type)
            {
                case AssetType.Scene:
                    return _assets.Where(data => Path.GetExtension(data.Value) == ".scene");
                case AssetType.Script:
                    return _assets.Where(data => Path.GetExtension(data.Value) == ".cs");
                case AssetType.Mesh:
                    return _assets.Where(data => Path.GetExtension(data.Value) == ".fbx");
                default:
                    return null;
            }
        }
    }
}

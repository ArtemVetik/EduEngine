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

            var fullPath = $"{_assetResolver.RootPath}\\{assetPath}.scene";

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

            var fullPath = $"{_assetResolver.RootPath}\\{assetPath}.scene";

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
                var type = ScriptParser.FindComponent(script.Value);

                if (type == scriptType)
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

        public static AssetType GetType(string guid)
        {
            _assets.TryGetValue(guid, out string assetPath);

            if (assetPath == null)
                return AssetType.Invalid;

            var extention = Path.GetExtension(assetPath);

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

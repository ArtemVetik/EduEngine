using Newtonsoft.Json;
using System.Reflection;

namespace EduEngine
{
    public static class SceneImporter
    {
        public static SceneData ToSceneData(Scene scene)
        {
            SceneData data = new SceneData();

            int id = 0;

            foreach (var item in scene.Objects)
            {
                var components = item.GetComponents<Component>().ToList();
                var componentsId = new List<int>(components.Count);

                foreach (var component in components)
                {
                    var parameters = new Dictionary<string, object>();
                    var publicFields = component.GetType().GetFields(BindingFlags.Public | BindingFlags.Instance);
                    var attributeFields = component.GetType().GetFields(BindingFlags.NonPublic | BindingFlags.Instance)
                        .Where(f => f.GetCustomAttribute<SerializeFieldAttribute>() != null);

                    var fields = publicFields.Concat(attributeFields);

                    foreach (var field in fields)
                    {
                        ParseParameter(field, component, out string filedName, out object fieldValue);
                        parameters.Add(filedName, fieldValue);
                    }

                    var type = component.GetType().Assembly == typeof(GameObject).Assembly ? component.GetType().FullName : "Script";

                    if (type == "Script")
                    {
                        var guid = GetScriptGUID(component.GetType());
                        parameters.Add("script_guid", guid);
                    }

                    componentsId.Add(id);
                    data.Components.Add(new SceneData.ComponentItem()
                    {
                        FileId = id++,
                        Type = type,
                        Parameters = parameters,
                    });
                }

                data.GameObjects.Add(new SceneData.GameObjectItem()
                {
                    FileId = id++,
                    Name = item.Name,
                    LocalPosition = item.Transform.Position,
                    LocalRotation = item.Transform.Rotation,
                    LocalScale = item.Transform.LocalScale,
                    Components = componentsId,
                });
            }

            return data;
        }

        public static void LoadScene(string guid, bool runtime)
        {
            var scenePath = AssetDataBase.GetAssetData(guid).LocalPath;

            if (Path.GetExtension(scenePath) == ".scene")
                scenePath = Path.Combine(
                   Path.GetDirectoryName(scenePath) ?? string.Empty,
                   Path.GetFileNameWithoutExtension(scenePath)
                );

            var fullPath = $"{AssetDataBase.AssetsPath}{scenePath}.scene";

            if (File.Exists(fullPath) == false)
                return;

            var sceneData = JsonConvert.DeserializeObject<SceneData>(File.ReadAllText(fullPath));

            if (sceneData == null)
                return;

            LoadScene(sceneData, guid, runtime);
        }

        public static void LoadScene(SceneData data, string guid, bool runtime)
        {
            var scene = new Scene(guid);
            SceneManager.OpenScene(scene);

            foreach (var goData in data.GameObjects)
            {
                GameObject go;
                if (runtime)
                    go = new GameObject();
                else
                    go = new EditorGameObject();

                go.Transform.Position = goData.LocalPosition;
                go.Transform.Rotation = goData.LocalRotation;
                go.Transform.LocalScale = goData.LocalScale;
                go.Name = goData.Name;

                foreach (var componentId in goData.Components)
                {
                    var cData = data.Components.First(c => c.FileId == componentId);

                    if (cData.Type == "Script")
                    {
                        var scriptGuid = cData.Parameters["script_guid"];

                        if (AssetDataBase.HasGUID((string)scriptGuid))
                        {
                            var scriptPath = AssetDataBase.GetAssetData((string)scriptGuid).GlobalPath;
                            var type = ScriptParser.FindComponent(scriptPath);
                            var parameters = ConvertAssetParameters(type, cData.Parameters);
                            var component = go.AddComponent(type, parameters);
                        }
                    }
                    else
                    {
                        var gameplayAssembly = Assembly.Load("GameplayFramework");
                        var type = gameplayAssembly.GetType(cData.Type);
                        var parameters = ConvertAssetParameters(type, cData.Parameters);
                        var component = go.AddComponent(type, parameters);
                    }
                }
            }
        }

        public static void SaveCurrentScene()
        {
            var sceneData = ToSceneData(SceneManager.CurrentScene);

            if (AssetDataBase.HasGUID(SceneManager.CurrentScene.GUID))
            {
                var scenePath = AssetDataBase.GetAssetData(SceneManager.CurrentScene.GUID).GlobalPath;
                File.WriteAllText(scenePath, JsonConvert.SerializeObject(sceneData, Formatting.Indented));
            }
            else
            {
                var sceneName = $"\\Scene_{DateTime.Now.Ticks}";
                CreateSceneFile(sceneName, sceneData);
                var guid = AssetDataBase.GetGUIDByLocalPath(sceneName + ".scene");
                LoadScene(guid, false);
            }
        }

        public static void CreateSceneFile(string sceneName, SceneData scene)
        {
            var fullPath = $"{AssetDataBase.AssetsPath}{sceneName}.scene";

            File.WriteAllText(fullPath, JsonConvert.SerializeObject(scene, Formatting.Indented));
            AssetDataBase.Resolve();
        }

        private static void ParseParameter(FieldInfo fieldInfo, object obj, out string fieldName, out object fieldValue)
        {
            fieldName = fieldInfo.Name;
            fieldValue = fieldInfo.GetValue(obj);

            if (fieldInfo.FieldType.IsSubclassOf(typeof(Asset)))
            {
                if (fieldValue != null)
                {
                    fieldValue = ((Asset)fieldValue).GUID;
                }
            }
        }

        private static Dictionary<string, object> ConvertAssetParameters(Type type, Dictionary<string, object> parameters)
        {
            var outParameters = new Dictionary<string, object>(parameters.Count);

            foreach (var parameter in parameters)
            {
                var field = type.GetField(parameter.Key, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);

                if (field != null)
                {
                    if (field.FieldType.IsSubclassOf(typeof(Asset)))
                    {
                        var assetGUID = (string)parameter.Value;

                        if (assetGUID != null && AssetDataBase.HasGUID(assetGUID))
                            outParameters[parameter.Key] = AssetDataBase.GetAssetData(assetGUID).Asset;
                        else
                            outParameters[parameter.Key] = null;
                    }
                    else
                    {
                        outParameters[parameter.Key] = parameter.Value;
                    }
                }
            }

            return outParameters;
        }

        private static string GetScriptGUID(Type scriptType)
        {
            var scripts = AssetDataBase.AllAssets.Where(asset => asset.Value.Type == AssetType.Script);

            foreach (var script in scripts)
            {
                var type = ScriptParser.FindComponent(script.Value.GlobalPath);

                if (type == scriptType)
                    return script.Key;
            }

            return null;
        }
    }
}
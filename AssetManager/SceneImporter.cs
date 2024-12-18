using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System.Reflection;

namespace EduEngine
{
    public static class SceneImporter
    {
        public static SceneData ToSceneData(Scene scene)
        {
            SceneData data = new SceneData();

            var objectIds = new Dictionary<GameObject, int>(scene.Objects.Count);
            int id = 0;

            foreach (var item in scene.Objects)
                objectIds.Add(item, id++);

            foreach (var item in scene.Objects)
            {
                var components = item.GetComponents<Component>().ToList();
                var componentsId = new List<int>(components.Count);

                foreach (var component in components)
                {
                    var parameters = new Dictionary<string, object>();
                    var fields = ComponentFields.FindAll(component);

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
                    FileId = objectIds[item],
                    Name = item.Name,
                    LocalPosition = item.Transform.LocalPosition,
                    LocalRotation = item.Transform.LocalRotation,
                    LocalScale = item.Transform.LocalScale,
                    Parent = (item.Parent != null && objectIds.TryGetValue(item.Parent, out int pId)) ? pId : -1,
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

            var objectIds = new Dictionary<int, GameObject>(data.GameObjects.Count);

            foreach (var goData in data.GameObjects)
            {
                GameObject go;
                if (runtime)
                    go = new GameObject();
                else
                    go = new EditorGameObject();

                objectIds.Add(goData.FileId, go);

                go.Transform.LocalPosition = goData.LocalPosition;
                go.Transform.LocalRotation = goData.LocalRotation;
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
                            var component = go.AddComponent(type, (component) => SetupParameters(component, ref parameters));
                        }
                    }
                    else
                    {
                        var gameplayAssembly = Assembly.Load("GameplayFramework");
                        var type = gameplayAssembly.GetType(cData.Type);
                        var parameters = ConvertAssetParameters(type, cData.Parameters);
                        var component = go.AddComponent(type, (component) => SetupParameters(component, ref parameters));
                    }
                }
            }

            foreach (var goData in data.GameObjects)
            {
                if (goData.Parent < 0)
                    continue;

                if (objectIds.TryGetValue(goData.Parent, out GameObject parent))
                    objectIds[goData.FileId].SetParent(parent);
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

        private static void SetupParameters(Component component, ref Dictionary<string, object> parameters)
        {
            if (parameters != null)
            {
                foreach (var parameter in parameters)
                {
                    var field = ComponentFields.FindField(component.GetType(), parameter.Key);

                    if (field != null)
                    {
                        if (field.FieldType.IsEnum)
                        {
                            field.SetValue(component, Enum.ToObject(field.FieldType, parameter.Value));
                        }
                        else
                        {
                            JObject jObj = parameter.Value as JObject;

                            if (jObj != null)
                            {
                                field.SetValue(component, jObj.ToObject(field.FieldType));
                            }
                            else
                            {
                                field.SetValue(component, Convert.ChangeType(parameter.Value, field.FieldType));
                            }
                        }
                    }
                }
            }
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
                var field = ComponentFields.FindField(type, parameter.Key);

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
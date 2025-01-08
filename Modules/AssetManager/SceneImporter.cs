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

            var goIds = new Dictionary<GameObject, int>(scene.Objects.Count);
            int fileId = 0;

            foreach (var item in scene.Objects)
                goIds.Add(item, fileId++);

            foreach (var sceneObject in scene.Objects)
            {
                var components = sceneObject.GetComponents<Component>();
                var componentsId = new List<int>(components.Length);

                foreach (var component in components)
                {
                    var parameters = GetParameters(component, goIds, out string componentType);
                    componentsId.Add(fileId);

                    data.Components.Add(new SceneData.ComponentItem()
                    {
                        FileId = fileId++,
                        Type = componentType,
                        Parameters = parameters,
                    });
                }

                data.GameObjects.Add(new SceneData.GameObjectItem()
                {
                    FileId = goIds[sceneObject],
                    Name = sceneObject.Name,
                    LocalPosition = sceneObject.Transform.LocalPosition,
                    LocalRotation = sceneObject.Transform.LocalRotation,
                    LocalScale = sceneObject.Transform.LocalScale,
                    Parent = (sceneObject.Parent != null && goIds.TryGetValue(sceneObject.Parent, out int parentId)) ? parentId : -1,
                    Components = componentsId,
                });
            }

            return data;
        }

        public static void LoadScene(string guid, bool runtime)
        {
            var scenePath = AssetDataBase.GetAssetData(guid).GlobalPath;

            if (File.Exists(scenePath) == false)
                throw new InvalidOperationException($"Scene {scenePath} not found");

            var sceneData = JsonConvert.DeserializeObject<SceneData>(File.ReadAllText(scenePath));
            LoadScene(sceneData, guid, runtime);
        }

        public static void LoadScene(SceneData data, string guid, bool runtime)
        {
            if (data == null)
                throw new InvalidOperationException($"Scene data is null");

            var scene = new Scene(guid);
            SceneManager.OpenScene(scene);

            var goIds = new Dictionary<int, GameObject>(data.GameObjects.Count);

            foreach (var goData in data.GameObjects)
            {
                GameObject go;
                if (runtime)
                    go = new GameObject();
                else
                    go = new EditorGameObject();

                go.Transform.LocalPosition = goData.LocalPosition;
                go.Transform.LocalRotation = goData.LocalRotation;
                go.Transform.LocalScale = goData.LocalScale;
                go.Name = goData.Name;

                goIds.Add(goData.FileId, go);
            }

            foreach (var goData in data.GameObjects)
            {
                if (goData.Parent < 0)
                    continue;

                if (goIds.TryGetValue(goData.Parent, out GameObject parent))
                    goIds[goData.FileId].SetParent(parent);
            }

            foreach (var goData in data.GameObjects)
            {
                GameObject go = goIds[goData.FileId];

                foreach (var componentId in goData.Components)
                {
                    var componentData = data.Components.First(c => c.FileId == componentId);

                    if (componentData.Type == "Script")
                    {
                        var scriptGuid = componentData.Parameters["script_guid"];

                        if (AssetDataBase.HasGUID((string)scriptGuid))
                        {
                            var scriptPath = AssetDataBase.GetAssetData((string)scriptGuid).GlobalPath;
                            var type = ScriptParser.FindComponent(scriptPath);
                            var component = go.AddComponent(type, (component) => SetParameters(component, componentData.Parameters, goIds));
                        }
                    }
                    else
                    {
                        var gameplayAssembly = Assembly.Load("GameplayFramework");
                        var type = gameplayAssembly.GetType(componentData.Type);
                        var component = go.AddComponent(type, (component) => SetParameters(component, componentData.Parameters, goIds));
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
            if (sceneName.StartsWith("\\") == false)
                sceneName = "\\" + sceneName;

            var fullPath = $"{AssetDataBase.AssetsPath}{sceneName}.scene";

            File.WriteAllText(fullPath, JsonConvert.SerializeObject(scene, Formatting.Indented));
            AssetDataBase.Resolve();
        }

        private static void SetParameters(Component component, Dictionary<string, object> parameters, Dictionary<int, GameObject> goIds)
        {
            if (parameters == null)
                return;

            foreach (var parameter in parameters)
            {
                var field = ComponentFields.FindField(component.GetType(), parameter.Key);

                if (field == null)
                    continue;

                var value = parameter.Value;

                if (value != null)
                {
                    if (field.FieldType.IsSubclassOf(typeof(Asset)))
                    {
                        var assetGUID = (string)parameter.Value;

                        if (AssetDataBase.HasGUID(assetGUID))
                            value = AssetDataBase.GetAssetData(assetGUID).Asset;
                        else
                            value = null;
                    }
                    else if (field.FieldType == typeof(GameObject))
                    {
                        var fileId = (int)Convert.ChangeType(parameter.Value, typeof(int));
                        value = goIds.TryGetValue(fileId, out GameObject go) ? go : null;
                    }
                }

                if (field.FieldType.IsEnum)
                {
                    field.SetValue(component, Enum.ToObject(field.FieldType, value));
                }
                else if (value is JObject jObj)
                {
                    field.SetValue(component, jObj.ToObject(field.FieldType));
                }
                else if (value is IConvertible)
                {
                    field.SetValue(component, Convert.ChangeType(value, field.FieldType));
                }
                else
                {
                    field.SetValue(component, value);
                }
            }
        }

        private static Dictionary<string, object> GetParameters(Component component, Dictionary<GameObject, int> objectIds, out string componentType)
        {
            var parameters = new Dictionary<string, object>();
            var fields = ComponentFields.FindAll(component);

            foreach (var field in fields)
            {
                var fieldValue = field.GetValue(component);

                if (fieldValue is Asset asset)
                {
                    fieldValue = asset.GUID;
                }
                else if (fieldValue is GameObject go)
                {
                    fieldValue = objectIds.TryGetValue(go, out int fileId) ? fileId : null;
                }

                parameters.Add(field.Name, fieldValue);
            }

            componentType = component.GetType().Assembly == typeof(GameObject).Assembly ? component.GetType().FullName : "Script";

            if (componentType == "Script")
            {
                var guid = ScriptParser.GetScriptGUID(component.GetType());
                parameters.Add("script_guid", guid);
            }

            return parameters;
        }
    }
}
using Newtonsoft.Json;
using System;
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
                        parameters.Add(field.Name, field.GetValue(component));
                    }

                    var type = component.GetType().Assembly == typeof(GameObject).Assembly ? component.GetType().FullName : "Script";

                    if (type == "Script")
                    {
                        var guid = AssetDataBase.GetScriptGUID(component.GetType());
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
            var scenePath = AssetDataBase.GetLocalPathByGUID(guid);
            AssetDataBase.LoadScene(scenePath, out SceneData? data);

            LoadScene(data, guid, runtime);
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

                        var scriptPath = AssetDataBase.GetGlobalPathByGUID((string)scriptGuid);
                        var type = ScriptParser.FindComponent(scriptPath);
                        var component = go.AddComponent(type, cData.Parameters);
                    }
                    else
                    {
                        var gameplayAssembly = Assembly.Load("GameplayFramework");
                        var type = gameplayAssembly.GetType(cData.Type);
                        var component = go.AddComponent(type, cData.Parameters);
                    }
                }
            }
        }

        public static void SaveCurrentScene()
        {
            var sceneData = ToSceneData(SceneManager.CurrentScene);

            if (AssetDataBase.HasGUID(SceneManager.CurrentScene.GUID))
            {
                var scenePath = AssetDataBase.GetGlobalPathByGUID(SceneManager.CurrentScene.GUID);
                File.WriteAllText(scenePath, JsonConvert.SerializeObject(sceneData));
            }
            else
            {
                var sceneName = $"\\Scene_{DateTime.Now.Ticks}";
                AssetDataBase.CreateScene(sceneName, sceneData);
                var guid = AssetDataBase.GetGUIDByLocalPath(sceneName + ".scene");
                LoadScene(guid, false);
            }
        }
    }
}

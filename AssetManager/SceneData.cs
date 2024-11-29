using System.Numerics;

namespace EduEngine
{
    public class SceneData
    {
        public SceneData()
        {
            GameObjects = new List<GameObjectItem>();
            Components = new List<ComponentItem>();
        }

        public List<GameObjectItem> GameObjects { get; set; }
        public List<ComponentItem> Components { get; set; }

        public class GameObjectItem
        {
            public int FileId { get; set; }
            public Vector3 LocalPosition { get; set; }
            public Quaternion LocalRotation { get; set; }
            public Vector3 LocalScale { get; set; }
            public string Name { get; set; }
            public required List<int> Components { get; set; }
        }

        public class ComponentItem
        {
            public required string Type { get; set; }
            public int FileId { get; set; }
            public required Dictionary<string, object> Parameters { get; set; }
        }
    }
}

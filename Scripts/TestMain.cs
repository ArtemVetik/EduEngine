using System.Numerics;

namespace EduEngine.Scripts
{
    public class TestMain
    {
        public static void Main()
        {
            var go = new EditorGameObject();
            go.Transform.Position = new Vector3(0, 5, -10);
            go.AddComponent<Root>();
            go.AddComponent<Camera>();
            go.AddComponent<TestCameraController>();
        }
    }
}

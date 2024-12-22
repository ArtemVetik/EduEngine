namespace EduEngine
{
    public class GameplayFramework
    {
        public static void Update()
        {
            SceneManager.CurrentScene?.Update();
        }

        public static void Render()
        {
            SceneManager.CurrentScene?.Render();
        }
    }
}

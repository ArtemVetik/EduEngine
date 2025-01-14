namespace EduEngine
{
    public class GameplayFramework
    {
        public static void Update()
        {
            SceneManager.CurrentScene?.Update();
        }

        public static void PhysicsUpdate()
        {
            SceneManager.CurrentScene?.PhysicsUpdate();
        }

        public static void Render()
        {
            SceneManager.CurrentScene?.Render();
        }

        public static void Destroy()
        {
            OverlayUI.Destroy();
        }
    }
}

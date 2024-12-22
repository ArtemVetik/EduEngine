namespace EduEngine
{
    public static class SceneManager
    {
        private static Scene _currentScene = null;

        public static Scene CurrentScene => _currentScene;

        internal static void OpenScene(Scene scene)
        {
            _currentScene?.Clear();
            _currentScene = scene;
        }
    }
}

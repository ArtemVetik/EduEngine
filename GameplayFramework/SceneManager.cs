namespace EduEngine
{
    public static class SceneManager
    {
        private static Scene _currentScene = null;

        public static Scene CurrentScene
        {
            get
            {
                _currentScene ??= new Scene();
                return _currentScene;
            }
        }
    }
}

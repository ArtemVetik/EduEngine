namespace EduEngine
{
    public class Scene
    {
        public IReadOnlyCollection<GameObject> Objects => _gameObjects;

        public void Clear()
        {
            foreach (var gameObject in _gameObjects)
                gameObject.Destroy();
        }

        internal void AddGameObject(GameObject gameObject)
        {
            _pendingAddGameObjects.Add(gameObject);
        }

        internal void RemoveGameObject(GameObject gameObject)
        {
            _pendingRemoveGameObjects.Add(gameObject);
        }

        internal void Update()
        {
            foreach (var pendingGameObject in _pendingAddGameObjects)
                _gameObjects.Add(pendingGameObject);

            _pendingAddGameObjects.Clear();

            foreach (var gameObject in _gameObjects)
                gameObject.Update();

            foreach (var pendingGameObject in _pendingRemoveGameObjects)
                _gameObjects.Remove(pendingGameObject);

            _pendingRemoveGameObjects.Clear();
        }

        private HashSet<GameObject> _gameObjects = new HashSet<GameObject>();
        private HashSet<GameObject> _pendingAddGameObjects = new HashSet<GameObject>();
        private HashSet<GameObject> _pendingRemoveGameObjects = new HashSet<GameObject>();
    }
}

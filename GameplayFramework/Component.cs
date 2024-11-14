namespace EduEngine
{
    public class Component
    {
        private readonly GameObject _gameObject;

        public Component(GameObject parent)
        {
            _gameObject = parent;
        }

        public GameObject GameObject => _gameObject;

        public virtual void Update() { }
    }
}

namespace EduEngine
{
    public class Component
    {
        private readonly GameObject _gameObject;
        private readonly Guid _guid;

        public Component(GameObject parent)
        {
            _gameObject = parent;
            _guid = Guid.NewGuid();
        }

        public Guid GUID => _guid;
        public GameObject GameObject => _gameObject;

        public virtual void OnAddComponent() { }
        public virtual void OnCreate() { }
        public virtual void OnGameStart() { }
        public virtual void OnDestroy() { }
        public virtual void Update() { }
        public virtual void PhysicsUpdate() { }
        public virtual void UpdateEditor() { }
        public virtual void OnRender() { }
        public virtual void OnScaleChanged() { }
        public virtual void OnParentChanged() { }
    }
}

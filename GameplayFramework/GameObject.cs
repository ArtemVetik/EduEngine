
using System.Reflection;

namespace EduEngine
{
    public class GameObject
    {
        private Guid _guid;
        private GameObject _parent;
        private List<Component> _components = new();
        private List<GameObject> _childs = new();

        public string Name;

        public GameObject()
        {
            if (SceneManager.CurrentScene == null)
                throw new InvalidOperationException();

            _guid = Guid.NewGuid();
            Transform = new Transform(this);
            Name = "New GameObject";

            SceneManager.CurrentScene.AddGameObject(this);
        }

        public Guid Guid => _guid;
        public GameObject Parent => _parent;
        public IReadOnlyList<GameObject> Childs => _childs;
        public Transform Transform { get; private set; }
        internal virtual bool IsRuntime { get; } = true;

        public void Destroy()
        {
            foreach (var component in _components)
            {
                if (IsRuntime)
                    component.OnDestroy();

                if (component is IDisposable disposable)
                    disposable.Dispose();
            }

            while (_childs.Count > 0)
            {
                var child = _childs[0];
                _childs.RemoveAt(0);

                child.SetParent(_parent);
            }

            SetParent(null);

            SceneManager.CurrentScene?.RemoveGameObject(this);
        }

        public void SetParent(GameObject parent)
        {
            _parent?._childs.Remove(this);
            _parent = parent;
            _parent?._childs.Add(this);
        }

        public void Update()
        {
            Transform.Update();

            foreach (var component in _components)
            {
                if (IsRuntime)
                    component.Update();
                else
                    component.UpdateEditor();
            }
        }

        public T AddComponent<T>() where T : Component
        {
            if (typeof(T) == typeof(Transform))
                return null;

            var component = Activator.CreateInstance(typeof(T), this);

            if (component == null || component as T == null)
                throw new InvalidOperationException();

            _components.Add((T)component);

            ((T)component).OnAddComponent();

            if (IsRuntime)
                ((T)component).OnCreate();

            return (T)component;
        }

        public object? AddComponent(Type type, Action<Component> initFields = null)
        {
            if (type.IsSubclassOf(typeof(Component)) == false)
                return null;

            if (type == typeof(Transform))
                return null;

            var component = (Component)Activator.CreateInstance(type, this);

            if (component == null)
                throw new InvalidOperationException();

            initFields?.Invoke(component);

            _components.Add(component);

            (component).OnAddComponent();

            if (IsRuntime)
                (component).OnCreate();

            return component;
        }

        public T GetComponent<T>() where T : Component
        {
            foreach (var component in _components)
            {
                if (component is T)
                    return (T)component;
            }

            return null;
        }

        public T[] GetComponents<T>() where T : Component
        {
            List<T> components = new List<T>();

            foreach (var component in _components)
            {
                if (component is T)
                    components.Add((T)component);
            }

            return components.ToArray();
        }

        public override bool Equals(object? obj)
        {
            if (obj is GameObject gameObject)
                return _guid.Equals(gameObject._guid);

            return false;
        }

        public override int GetHashCode()
        {
            return _guid.GetHashCode();
        }

        internal void RemoveComponent(Component component)
        {
            if (component is IDisposable disposable)
                disposable.Dispose();

            _components.Remove(component);
        }

        internal void Render()
        {
            foreach (var component in _components)
                component.OnRender();
        }
    }
}

using System.Numerics;

namespace EduEngine
{
    public class GameObject
    {
        internal static object Lock = new object();

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
        public bool IsAlive { get; private set; } = true;
        internal virtual bool IsRuntime { get; } = true;

        public void Destroy()
        {
            lock (Lock)
            {
                foreach (var component in _components)
                {
                    if (IsRuntime)
                        component.OnDestroy();

                    if (component is IDisposable disposable)
                        disposable.Dispose();
                }
                _components.Clear();

                var childsCopy = _childs.ToArray();
                
                foreach (var child in childsCopy)
                {
                    child.Destroy();
                }

                SetParent(null);

                SceneManager.CurrentScene?.RemoveGameObject(this);
                IsAlive = false;
            }
        }

        public void SetParent(GameObject? parent, bool stayWorldPosition = false)
        {
            Vector3 position = Vector3.Zero;
            Quaternion rotation = Quaternion.Identity;

            if (stayWorldPosition)
            {
                position = Transform.Position;
                rotation = Transform.Rotation;
            }

            _parent?._childs.Remove(this);
            _parent = parent;
            _parent?._childs.Add(this);

            if (stayWorldPosition)
            {
                Transform.Position = position;
                Transform.Rotation = rotation;
            }
        }

        public void Update()
        {
            lock (Lock)
            {
                var dirtyScale = Transform.DirtyScale;

                Transform.Update();

                if (dirtyScale)
                    SendScaleChanged(this);

                foreach (var component in _components)
                {
                    if (IsRuntime)
                        component.Update();
                    else
                        component.UpdateEditor();
                }
            }
        }

        public T AddComponent<T>() where T : Component
        {
            lock (Lock)
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
        }

        public object? AddComponent(Type type, Action<Component> initFields = null)
        {
            lock (Lock)
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

        public T[] GetComponentsInChildren<T>() where T : Component
        {
            List<T> components = new List<T>();
            GetComponentsInChildren(this, components);

            return components.ToArray();
        }

        public T[] GetComponentsInParent<T>() where T : Component
        {
            List<T> components = new List<T>();

            var parent = this;

            while (parent != null)
            {
                foreach (var component in parent._components)
                {
                    if (component is T target)
                        components.Add(target);
                }

                parent = parent._parent;
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
            lock (Lock)
            {
                if (component is IDisposable disposable)
                    disposable.Dispose();

                _components.Remove(component);
            }
        }

        internal void Render()
        {
            lock (Lock)
            {
                foreach (var component in _components)
                    component.OnRender();
            }
        }

        private void GetComponentsInChildren<T>(GameObject go, List<T> components) where T : Component
        {
            foreach (var component in go._components)
            {
                if (component is T target)
                    components.Add(target);
            }

            foreach (var child in go.Childs)
                GetComponentsInChildren(child, components);
        }

        private void SendScaleChanged(GameObject go)
        {
            foreach(var component in go._components)
                component.OnScaleChanged();

            foreach (var child in go._childs)
                SendScaleChanged(child);
        }
    }
}

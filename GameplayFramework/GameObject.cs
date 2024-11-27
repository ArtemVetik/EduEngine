
using System.Reflection;

namespace EduEngine
{
    public class GameObject
    {
        private Guid _guid;
        private List<Component> _components = new();

        public GameObject()
        {
            _guid = Guid.NewGuid();
            Transform = new Transform(this);

            SceneManager.CurrentScene.AddGameObject(this);
        }

        internal virtual bool EnableCallbacks { get; } = true;

        public void Destroy()
        {
            foreach (var component in _components)
            {
                if (EnableCallbacks)
                    component.OnDestroy();

                if (component is IDisposable disposable)
                    disposable.Dispose();
            }

            SceneManager.CurrentScene.RemoveGameObject(this);
        }

        public Transform Transform { get; private set; }

        public void Update()
        {
            Transform.Update();

            foreach (var component in _components)
                component.Update();
        }

        public T AddComponent<T>() where T : Component
        {
            if (typeof(T) == typeof(Transform))
                return null;

            var component = Activator.CreateInstance(typeof(T), this);

            if (component == null || component as T == null)
                throw new InvalidOperationException();

            _components.Add((T)component);

            ((T)component).OnDeserialized();

            if (EnableCallbacks)
                ((T)component).OnCreate();

            return (T)component;
        }

        public object? AddComponent(Type type, IReadOnlyDictionary<string, object> parameters = null)
        {
            if (type.IsSubclassOf(typeof(Component)) == false)
                return null;

            if (type == typeof(Transform))
                return null;

            var component = Activator.CreateInstance(type, this);

            if (component == null)
                throw new InvalidOperationException();

            if (parameters != null)
            {
                foreach (var parameter in parameters)
                {
                    var field = type.GetField(parameter.Key, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance);
                    
                    if (field != null)
                        field.SetValue(component, Convert.ChangeType(parameter.Value, field.FieldType));
                }
            }

            _components.Add((Component)component);

            ((Component)component).OnDeserialized();

            if (EnableCallbacks)
                ((Component)component).OnCreate();

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

        internal void Render()
        {
            foreach (var component in _components)
                component.OnRender();
        }
    }
}

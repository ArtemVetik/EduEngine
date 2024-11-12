
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
            return (T)component;
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
    }
}

namespace EduEngine
{
    public class Component
    {
        private readonly GameObject _parent;

        public Component(GameObject parent)
        {
            _parent = parent;
        }

        public GameObject Parent => _parent;

        public virtual void Update() { }
    }
}

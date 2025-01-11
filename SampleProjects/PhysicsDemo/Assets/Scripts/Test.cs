using EduEngine;

namespace PhysicsDemo
{
    internal class Test : Component
    {
        [SerializeField] private Material _material;

        private Renderer _renderer;

        public Test(GameObject parent) : base(parent)
        {
        }

        public override void OnGameStart()
        {
            _renderer = GameObject.GetComponent<Renderer>();
        }

        public override void Update()
        {
            if (Input.Runtime.KeyDown(KeyCode.R))
            {
                _renderer.SetMaterial(_material);
                _renderer.SetMaterial(null);
            }
        }
    }
}

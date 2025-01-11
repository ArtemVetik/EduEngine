using EduEngine;

namespace PhysicsDemo
{
    internal class CollisionEffect : Component, IColliderCallbacks
    {
        [SerializeField] private Material _collideMaterial;

        private Material _defaultMaterial;
        private Renderer _renderer;
        private bool _update = false;
        private float _timer = 0.0f;

        public CollisionEffect(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _renderer = GameObject.GetComponent<Renderer>();
            _defaultMaterial = _renderer.Material;
        }

        public override void Update()
        {
            if (_update == false)
                return;

            _timer -= EduTime.DeltaTime;

            if (_timer < 0.0f)
            {
                _renderer.SetMaterial(_defaultMaterial);
                _update = false;
            }
        }

        public void OnCollisionEnter(CollisionData collisionData)
        {
            _renderer.SetMaterial(_collideMaterial);
            _timer = 0.5f;
            _update = true;
        }

        public void OnCollisionExit(CollisionData collisionData)
        {

        }

        public void OnTriggerEnter(Collider other)
        {

        }

        public void OnTriggerExit(Collider other)
        {

        }

        public void SetMaterial(Material material)
        {
            _collideMaterial = material;
        }
    }
}

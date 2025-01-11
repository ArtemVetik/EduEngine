using EduEngine;
using System.Numerics;

namespace PhysicsDemo
{
    internal class SphereGun : Component
    {
        [SerializeField] private GameObject _camera;
        [SerializeField] private SharedMesh _sphereMesh;
        [SerializeField] private SharedMesh _cubeMesh;
        [SerializeField] private Material _material;

        private List<GameObject> _spawnedInstances = new();
        private float _firePower = 100.0f;
        private bool _sphereProjectile = true;

        public SphereGun(GameObject parent)
            : base(parent)
        { }

        public int SpawnedInstancesCount => _spawnedInstances.Count;
        public bool SphereProjectile => _sphereProjectile;

        public override void Update()
        {
            if (Input.Runtime.MousePosition.X < 0 || Input.Runtime.MousePosition.Y < 0 ||
                Input.Runtime.MousePosition.X > Screen.Size.X || Input.Runtime.MousePosition.Y > Screen.Size.Y)
                return;

            if (Input.Runtime.KeyDown(KeyCode.SPACE))
            {
                if (_sphereProjectile)
                    SpawnSphere();
                else
                    SpawnCube();
            }
        }

        public void ClearInstances()
        {
            foreach (var instance in _spawnedInstances)
                instance.Destroy();

            _spawnedInstances.Clear();
        }

        public void SetPower(float power)
        {
            _firePower = power;
        }

        public void SetSphereProjectile(bool value)
        {
            _sphereProjectile = value;
        }

        private void SpawnSphere()
        {
            var instance = new GameObject();
            instance.Transform.Position = _camera.Transform.Position;
            instance.Transform.LocalScale = Vector3.One * 0.5f;

            var renderer = instance.AddComponent<Renderer>();
            renderer.SetMesh(_sphereMesh);
            renderer.SetMaterial(_material);

            var collider = instance.AddComponent<SphereCollider>();
            collider.Setup(1.0f, 1);

            var body = instance.AddComponent<RigidBody>();
            body.AddForce(_camera.Transform.Forward * _firePower, ForceMode.VELOCITY_CHANGE);

            _spawnedInstances.Add(instance);
        }

        private void SpawnCube()
        {
            var instance = new GameObject();
            instance.Transform.Position = _camera.Transform.Position;

            var renderer = instance.AddComponent<Renderer>();
            renderer.SetMesh(_cubeMesh);
            renderer.SetMaterial(_material);
            
            instance.AddComponent<BoxCollider>();

            var body = instance.AddComponent<RigidBody>();
            body.AddForce(_camera.Transform.Forward * _firePower, ForceMode.VELOCITY_CHANGE);

            _spawnedInstances.Add(instance);
        }
    }
}

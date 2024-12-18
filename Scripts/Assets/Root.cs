using System.Numerics;

namespace EduEngine.Scripts
{
    public class Root : Component
    {
        private List<GameObject> _spawned = new List<GameObject>();
        private SharedMesh _mesh;

        public Root(GameObject parent)
            : base(parent)
        {
            var mesh = AssetDataBase.AllAssets.First(asset => asset.Value.Type == AssetType.Mesh);
            _mesh = mesh.Value.Asset as SharedMesh;
        }

        public override void Update()
        {
            if (Input.Runtime.KeyDown(KeyCode.P))
            {
                var cube = new GameObject();
                cube.Name = "Cube";
                cube.Transform.Position = new Vector3(0, 0, 0);
                cube.AddComponent<SamplePhysicsObject>();
                cube.AddComponent<Renderer>().SetMesh(_mesh);
                var collider = cube.AddComponent<BoxCollider>();
                collider.Setup(10.2f, 1.2f, 10.2f);
                collider.SetTrigger(true);
                cube.AddComponent<RigidBody>().IsStatic = true;

                _spawned.Add(cube);
            }
            if (Input.Runtime.KeyDown(KeyCode.O))
            {
                float radius = 0.2f + (float)Random.Shared.NextDouble() * 1.5f;
                var sphere = new GameObject();
                sphere.Name = "Sphere";
                sphere.Transform.Position = new Vector3(Random.Shared.Next(-10, 10), Random.Shared.Next(0, 20), Random.Shared.Next(-10, 10));
                sphere.AddComponent<Renderer>();
                sphere.AddComponent<SamplePhysicsObject>();
                var collider = sphere.AddComponent<SphereCollider>();
                collider.Setup(radius + 0.1f);
                sphere.AddComponent<RigidBody>();

                _spawned.Add(sphere);
            }
            if (Input.Runtime.KeyDown(KeyCode.C))
            {
                foreach (var item in _spawned)
                    item.Destroy();

                _spawned.Clear();
            }
        }
    }
}

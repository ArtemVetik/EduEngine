using System.Numerics;

namespace EduEngine.Scripts
{
    public class Root : Component
    {
        private List<GameObject> _spawned = new List<GameObject>();

        public Root(GameObject parent)
            : base(parent)
        { }

        public override void Update()
        {
            if (Input.KeyDown(KeyCode.P))
            {
                TestMeshGenerator.GenerateCube(out Vertex[] vertices, out uint[] indices);
                var cube = new GameObject();
                cube.Transform.Position = new Vector3(Random.Shared.Next(-10, 10), Random.Shared.Next(0, 20), Random.Shared.Next(-10, 10));
                cube.AddComponent<Renderer>().SetMesh(new MeshData()
                {
                    Vertices = vertices.ToList(),
                    Indices32 = indices.ToList(),
                });
                cube.AddComponent<BoxCollider>().Setup(1.2f, 1.2f, 1.2f);
                cube.AddComponent<RigidBody>();
                _spawned.Add(cube);
            }
            if (Input.KeyDown(KeyCode.O))
            {
                float radius = 0.2f + (float)Random.Shared.NextDouble() * 1.5f;
                TestMeshGenerator.GenerateSphere(32, 32, radius, out Vertex[] vertices, out uint[] indices);
                var sphere = new GameObject();
                sphere.Transform.Position = new Vector3(Random.Shared.Next(-10, 10), Random.Shared.Next(0, 20), Random.Shared.Next(-10, 10));
                sphere.AddComponent<Renderer>().SetMesh(new MeshData()
                {
                    Vertices = vertices.ToList(),
                    Indices32 = indices.ToList(),
                });
                sphere.AddComponent<SphereCollider>().Setup(radius + 0.1f);
                sphere.AddComponent<RigidBody>();
                _spawned.Add(sphere);
            }
            if (Input.KeyDown(KeyCode.C))
            {
                foreach (var item in _spawned)
                    item.Destroy();

                _spawned.Clear();
            }
        }
    }
}


namespace EduEngine.Scripts
{
    internal class ProcedureMesh : Component
    {
        [SerializeField] private bool _cube = true;
        [SerializeField] private float _radius = 1.0f;

        public ProcedureMesh(GameObject parent) : base(parent)
        { }

        public override void OnCreate()
        {
            Vertex[] vertices;
            uint[] indices;

            if (_cube)
                TestMeshGenerator.GenerateCube(out vertices, out indices);
            else
                TestMeshGenerator.GenerateSphere(32, 32, _radius, out vertices, out indices);

            var mesh = GameObject.GetComponent<Renderer>();
            mesh.SetMesh(new MeshData()
            {
                Vertices = vertices.ToList(),
                Indices32 = indices.ToList(),
            });
        }

        public void SetupMesh(bool cube, float radius)
        {
            _cube = cube;
            _radius = radius;

            OnCreate();
        }
    }
}

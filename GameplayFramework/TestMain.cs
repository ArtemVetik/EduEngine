using System.Numerics;

namespace EduEngine
{
    public class TestMain
    {
        private static GameObject _go1;
        private static GameObject _go2;
        private static GameObject _camGo;

        public static void TestInitialize()
        {
            GenerateCube(out Vertex[] vertices, out uint[] indices);
            GenerateSphere(16, 16, 1.5f, out Vertex[] svertices, out uint[] sindices);
            
            _go1 = new GameObject();
            _go1.Transform.Position = Vector3.UnitY * 10;
            var renderer = _go1.AddComponent<Renderer>();
            renderer.SetMesh(new MeshData()
            {
                Vertices = vertices.ToList(),
                Indices32 = indices.ToList(),
            });
            var boxCollider = _go1.AddComponent<BoxCollider>();
            boxCollider.Setup(5.5f, 5.5f, 2.5f);
            boxCollider.Setup(1.5f, 1.5f, 1.5f);
            var rigidBody = _go1.AddComponent<RigidBody>();


            _go2 = new GameObject();
            _go2.Transform.Position = Vector3.UnitY * 15;
            var renderer2 = _go2.AddComponent<Renderer>();
            renderer2.SetMesh(new MeshData()
            {
                Vertices = svertices.ToList(),
                Indices32 = sindices.ToList(),
            });
            var boxCollider2 = _go2.AddComponent<SphereCollider>();
            boxCollider2.Setup(2);
            var rigidBody2 = _go2.AddComponent<RigidBody>();

            _camGo = new GameObject();
            _camGo.Transform.Position = new Vector3(0, 5, -10);
            _camGo.Transform.Rotation = Quaternion.Identity;
            _camGo.AddComponent<Camera>();
        }

        public static void TestUpdate()
        {
            _go1.Update();
            _go2.Update();
            _camGo.Update();
        }

        #region GeometryGenerator

        public static void GenerateCube(out Vertex[] vertices, out uint[] indices)
        {
            vertices = new Vertex[]
            {
                new Vertex(new Vector3(-0.5f, -0.5f, -0.5f), Vector3.Zero, Vector3.Zero, new Vector2(0, 0)),
                new Vertex(new Vector3(0.5f, -0.5f, -0.5f), Vector3.Zero, Vector3.Zero, new Vector2(0, 1)),
                new Vertex(new Vector3(0.5f,  0.5f, -0.5f), Vector3.Zero, Vector3.Zero, new Vector2(1, 0)),
                new Vertex(new Vector3(-0.5f,  0.5f, -0.5f), Vector3.Zero, Vector3.Zero, new Vector2(1, 1)),
                new Vertex(new Vector3(-0.5f, -0.5f,  0.5f), Vector3.Zero, Vector3.Zero, new Vector2(0, 0)),
                new Vertex(new Vector3(0.5f, -0.5f,  0.5f), Vector3.Zero, Vector3.Zero, new Vector2(0, 1)),
                new Vertex(new Vector3(0.5f,  0.5f,  0.5f), Vector3.Zero, Vector3.Zero, new Vector2(1, 0)),
                new Vertex(new Vector3(-0.5f,  0.5f,  0.5f), Vector3.Zero, Vector3.Zero, new Vector2(1, 1)),
            };

            indices = new uint[]
            {
                0, 2, 1, 0, 3, 2,
                4, 5, 6, 4, 6, 7,
                0, 1, 5, 0, 5, 4,
                3, 7, 6, 3, 6, 2,
                0, 4, 7, 0, 7, 3,
                1, 2, 6, 1, 6, 5
            };
        }

        public static void GenerateSphere(int latitudes, uint longitudes, float radius, out Vertex[] vertices, out uint[] indices)
        {
            List<Vertex> vertexList = new List<Vertex>();
            List<uint> indexList = new List<uint>();

            for (int lat = 0; lat <= latitudes; lat++)
            {
                float theta = (float)(lat * Math.PI / latitudes);
                float sinTheta = (float)Math.Sin(theta);
                float cosTheta = (float)Math.Cos(theta);

                for (int lon = 0; lon <= longitudes; lon++)
                {
                    float phi = (float)(lon * 2 * Math.PI / longitudes);
                    float sinPhi = (float)Math.Sin(phi);
                    float cosPhi = (float)Math.Cos(phi);

                    float x = radius * sinTheta * cosPhi;
                    float y = radius * cosTheta;
                    float z = radius * sinTheta * sinPhi;

                    vertexList.Add(new Vertex(new Vector3(x, y, z), Vector3.Zero, Vector3.Zero, new Vector2(lat / (float)latitudes, lon / (float)longitudes)));
                }
            }

            for (uint lat = 0; lat < latitudes; lat++)
            {
                for (uint lon = 0; lon < longitudes; lon++)
                {
                    uint first = lat * (longitudes + 1) + lon;
                    uint second = first + longitudes + 1;

                    indexList.Add(first);
                    indexList.Add(second);
                    indexList.Add(first + 1);

                    indexList.Add(second);
                    indexList.Add(second + 1);
                    indexList.Add(first + 1);
                }
            }

            vertices = vertexList.ToArray();
            indices = indexList.ToArray();
        }

        #endregion
    }
}

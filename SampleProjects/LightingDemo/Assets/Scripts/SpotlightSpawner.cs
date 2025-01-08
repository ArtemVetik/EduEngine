using EduEngine;
using System.Numerics;

namespace LightingDemo
{
    public class SpotlightSpawner : Component
    {
        [SerializeField] private SharedMesh _cylinderMesh;
        [SerializeField] private Material _material;
        [SerializeField] private int _lightRows = 10;
        [SerializeField] private int _lightColumns = 10;
        [SerializeField] private int _spawnArea = 40;

        private List<GameObject> _lightInstances = new();
        private List<GameObject> _staticObjects = new();

        public SpotlightSpawner(GameObject parent)
            : base(parent)
        { }

        public int LightCount => _lightRows * _lightColumns;

        public override void OnCreate()
        {
            var xShift = (_spawnArea * 2.0f) / (_lightRows - 1);
            var zShift = (_spawnArea * 2.0f) / (_lightColumns - 1);

            for (int z = 0; z < _lightColumns; z++)
            {
                for (int x = 0; x < _lightRows; x++)
                {
                    var instance = new GameObject();
                    instance.Name = $"Spotlight-{x + z}";

                    instance.Transform.Position = GameObject.Transform.Position + new Vector3(-_spawnArea + xShift * x, 15, -_spawnArea + zShift * z);
                    instance.AddComponent<SpotlightObject>();

                    _lightInstances.Add(instance);
                }
            }

            var gridSize = 6;
            var area = 0.9f * _spawnArea;
            var step = (float)(area * 2) / (gridSize - 1);

            for (int z = 0; z < gridSize; z++)
            {
                for (int x = 0; x < gridSize; x++)
                {
                    var instance = new GameObject();
                    instance.Name = $"Cylinder-{x + z}";
                    instance.Transform.Position = GameObject.Transform.Position + new Vector3(-area + step * x, 4, -area + step * z);
                    instance.Transform.LocalScale = new Vector3(2, 4, 2);

                    var renderer = instance.AddComponent<Renderer>();
                    renderer.SetMesh(_cylinderMesh);
                    renderer.SetMaterial(_material);

                    _staticObjects.Add(instance);
                }
            }
        }
    }
}

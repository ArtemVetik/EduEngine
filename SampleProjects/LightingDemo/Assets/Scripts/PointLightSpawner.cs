using EduEngine;
using System.Numerics;

namespace LightingDemo
{
    public class PointLightSpawner : Component
    {
        [SerializeField] private SharedMesh _sphereMesh;
        [SerializeField] private Material _material;
        [SerializeField] private int _spawnCount = 100;
        [SerializeField] private int _spawnArea = 40;

        private List<GameObject> _lightInstances = new();
        private List<GameObject> _staticObjects = new();

        public PointLightSpawner(GameObject parent)
            : base(parent)
        { }

        public int LightCount => _spawnCount;

        public override void OnCreate()
        {
            for (int i = 0; i < _spawnCount; i++)
            {
                var instance = new GameObject();
                instance.Name = $"PointLight-{i}";

                var x = Random.Shared.Next(-_spawnArea, _spawnArea);
                var z = Random.Shared.Next(-_spawnArea, _spawnArea);
                instance.Transform.Position = GameObject.Transform.Position + new Vector3(x, 1, z);

                var renderer = instance.AddComponent<Renderer>();
                renderer.SetMesh(_sphereMesh);
                renderer.SetMaterial(_material);

                instance.AddComponent<PointLightObject>();

                _lightInstances.Add(instance);
            }

            var gridSize = 4;
            var area = 0.9f * _spawnArea;
            var step = (float)(area * 2) / (gridSize - 1);

            for (int z = 0; z < gridSize; z++)
            {
                for (int x = 0; x < gridSize; x++)
                {
                    var instance = new GameObject();
                    instance.Name = $"Sphere-{x + z}";
                    instance.Transform.Position = GameObject.Transform.Position + new Vector3(-area + step * x, 3, -area + step * z);
                    instance.Transform.LocalScale = Vector3.One * 3.0f;

                    var renderer = instance.AddComponent<Renderer>();
                    renderer.SetMesh(_sphereMesh);
                    renderer.SetMaterial(_material);

                    _staticObjects.Add(instance);
                }
            }
        }
    }
}

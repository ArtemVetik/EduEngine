using EduEngine;
using System.Numerics;

namespace PhysicsDemo
{
    internal class BoxSpawner : Component
    {
        [SerializeField] private SharedMesh _boxMesh;
        [SerializeField] private Material _defaultMaterial;
        [SerializeField] private Material _collideMaterial;
        [SerializeField] private int _width = 8;
        [SerializeField] private int _depth = 8;
        [SerializeField] private int _height = 8;

        public BoxSpawner(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            for (int z = 0; z < _depth; z++)
            {
                for (int y = 0; y < _height; y++)
                {
                    for (int x = 0; x < _width; x++)
                    {
                        var box = new GameObject();
                        box.Transform.Position = GameObject.Transform.Position + new Vector3(x, y, z);

                        var renderer = box.AddComponent<Renderer>();
                        renderer.SetMesh(_boxMesh);
                        renderer.SetMaterial(_defaultMaterial);
                        renderer.SetTextureScale(Vector2.One * 4);

                        box.AddComponent<BoxCollider>();
                        box.AddComponent<RigidBody>();

                        box.AddComponent<CollisionEffect>().SetMaterial(_collideMaterial);
                    }
                }
            }
        }
    }
}

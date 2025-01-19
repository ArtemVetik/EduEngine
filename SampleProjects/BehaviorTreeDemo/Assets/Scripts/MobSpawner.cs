using EduEngine;
using System.Numerics;

namespace BehaviorTreeDemo
{
    public class MobSpawner : Component
    {
        [SerializeField] private Vector2 _spawnRange;
        [SerializeField] private int _spawnCount;
        [SerializeField] private SharedMesh _smileMesh;
        [SerializeField] private SharedMesh _cryMesh;
        [SerializeField] private Material _material;
        [SerializeField] private Texture _deadEffectTexture;

        private readonly List<Mob> _mobs = new();

        public MobSpawner(GameObject parent)
            : base(parent)
        { }

        public IReadOnlyList<Mob> Mobs => _mobs;

        public void Spawn()
        {
            for (int i = 0; i < _spawnCount; i++)
            {
                var collider = new GameObject();
                collider.AddComponent<SphereCollider>();

                var rendererObject = new GameObject();
                rendererObject.Transform.LocalScale = Vector3.One * 4.0f;
                rendererObject.Transform.Rotation = Quaternion.CreateFromAxisAngle(Vector3.UnitY, Random.Shared.NextSingle() * MathF.PI);
                
                var renderer = rendererObject.AddComponent<Renderer>();
                renderer.SetMesh(_smileMesh);
                renderer.SetMaterial(_material);
                renderer.SetTextureScale(new Vector2(1, -1));

                var instance = new GameObject();
                instance.Name = $"Mob-{i}";
                instance.Transform.LocalScale = Vector3.One * 0.5f;

                var x = (Random.Shared.NextSingle() * 2 - 1) * _spawnRange.X;
                var z = (Random.Shared.NextSingle() * 2 - 1) * _spawnRange.Y;
                instance.Transform.Position = new Vector3(x, 3, z);
                collider.SetParent(instance);
                rendererObject.SetParent(instance);

                var body = instance.AddComponent<RigidBody>();
                body.FreezeRotation(true, false, true);

                var mob = instance.AddComponent<Mob>();

                _mobs.Add(mob);
            }
        }

        public void Initialize(Character[] characters)
        {
            foreach (var mob in _mobs)
                mob.Initialize(characters, _smileMesh, _cryMesh, _deadEffectTexture);
        }
    }
}

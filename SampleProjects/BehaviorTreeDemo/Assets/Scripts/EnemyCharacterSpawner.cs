using EduEngine;
using System.Numerics;

namespace BehaviorTreeDemo
{
    public class EnemyCharacterSpawner : Component
    {
        [SerializeField] private Vector2 _spawnRange;
        [SerializeField] private int _spawnCount;
        [SerializeField] private SharedMesh _mesh;
        [SerializeField] private Material _material;
        [SerializeField] private GameObject _playerCharacter;

        private readonly List<Character> _characters = new();

        public EnemyCharacterSpawner(GameObject parent)
            : base(parent)
        { }

        public IReadOnlyList<Character> Characters => _characters;

        public void Spawn()
        {
            for (int i = 0; i < _spawnCount; i++)
            {
                var collider = new GameObject();
                collider.Transform.Rotation = Quaternion.CreateFromYawPitchRoll(0, 0, MathF.PI / 2.0f);
                collider.AddComponent<CapsuleCollider>();

                var rendererObject = new GameObject();
                rendererObject.Transform.LocalScale = Vector3.One * 2;
                rendererObject.Transform.Position = Vector3.UnitY * -1;
                var renderer = rendererObject.AddComponent<Renderer>();
                renderer.SetMesh(_mesh);
                renderer.SetMaterial(_material);

                var instance = new GameObject();
                instance.Name = $"Enemy-{i}";

                var x = (Random.Shared.NextSingle() * 2 - 1) * _spawnRange.X;
                var z = (Random.Shared.NextSingle() * 2 - 1) * _spawnRange.Y;
                instance.Transform.Position = new Vector3(x, 3, z);
                collider.SetParent(instance);
                rendererObject.SetParent(instance);

                var body = instance.AddComponent<RigidBody>();
                body.FreezeRotation(true, false, true);

                var enemyCharacter = instance.AddComponent<EnemyCharacter>();

                _characters.Add(enemyCharacter);
            }

            _characters.Add(_playerCharacter.GetComponent<Character>());
        }

        public void Initialize(Mob[] mobs)
        {
            int enemyIndex = 0;

            foreach (var character in _characters)
            {
                if (character is EnemyCharacter enemy)
                {
                    enemy.Initialize(enemyIndex++, _characters.Except(new Character[] { character }).ToArray(), mobs);
                }
            }
        }
    }
}

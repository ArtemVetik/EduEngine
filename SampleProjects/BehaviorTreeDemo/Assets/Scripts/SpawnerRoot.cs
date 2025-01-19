using EduEngine;

namespace BehaviorTreeDemo
{
    public class SpawnerRoot : Component
    {
        [SerializeField] private GameObject _mobSpawnerObject;
        [SerializeField] private GameObject _characterSpawnerObject;

        private MobSpawner _mobSpawner;
        private EnemyCharacterSpawner _characterSpawner;

        public SpawnerRoot(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _mobSpawner = _mobSpawnerObject.GetComponent<MobSpawner>();
            _characterSpawner = _characterSpawnerObject.GetComponent<EnemyCharacterSpawner>();

            _mobSpawner.Spawn();
            _characterSpawner.Spawn();
            _mobSpawner.Initialize(_characterSpawner.Characters.ToArray());
            _characterSpawner.Initialize(_mobSpawner.Mobs.ToArray());
        }
    }
}

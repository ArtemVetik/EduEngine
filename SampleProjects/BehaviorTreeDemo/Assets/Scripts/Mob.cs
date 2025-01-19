using EduEngine;
using BananaParty.BehaviorTree;

namespace BehaviorTreeDemo
{
    public class Mob : Component, ICombatant
    {
        private Character[] _characters;
        private INode _behaviorTree;
        private SharedVariable<Character> _nearbyCharacter;
        private Renderer _renderer;
        private SharedMesh _smileMesh;
        private SharedMesh _cryMesh;
        private Texture _effectTexture;

        public int PowerLevel => 1;

        public Mob(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _renderer = GameObject.GetComponentsInChildren<Renderer>().First();
            _nearbyCharacter = new SharedVariable<Character>();

            _behaviorTree = new Repeat(new ReactiveSelector(new INode[]
                {
                    new ReactiveSequence(new INode[]
                    {
                        new GetNearbyObject<Character>(GameObject, _characters, _nearbyCharacter, 5.0f),
                        new FreeFromCharacter(GameObject, _nearbyCharacter)
                    }),
                    new Wait(1.0f)
                })
            );
        }

        public override void Update()
        {
            _behaviorTree.Execute(EduTime.DeltaTime);

            if (_nearbyCharacter.Value != null)
                _renderer.SetMesh(_cryMesh);
            else
                _renderer.SetMesh(_smileMesh);
        }

        public void Initialize(Character[] characters, SharedMesh smileMesh, SharedMesh cryMesh, Texture effectTexture)
        {
            _characters = characters;
            _smileMesh = smileMesh;
            _cryMesh = cryMesh;
            _effectTexture = effectTexture;
        }

        public void Die()
        {
            var effectObject = new GameObject();
            effectObject.Transform.Position = GameObject.Transform.Position;
            effectObject.AddComponent<ParticleSystem>();
            var effect = effectObject.AddComponent<MobDeadParticleSystem>();
            effect.Initialize(_effectTexture);

            GameObject.Destroy();
        }
    }
}

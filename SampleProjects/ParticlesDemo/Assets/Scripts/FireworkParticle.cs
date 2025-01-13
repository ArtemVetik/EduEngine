using EduEngine;

namespace ParticlesDemo
{
    internal class FireworkParticle : Component
    {
        [SerializeField] private float _waitTime;
        
        private ParticleSystem _particleSystem;

        public FireworkParticle(GameObject parent) : base(parent)
        { }

        public override void OnGameStart()
        {
            _particleSystem = GameObject.GetComponent<ParticleSystem>();
            _particleSystem.SetEmissionState(false);
        }

        public override void Update()
        {
            if (_waitTime < 0)
                return;

            _waitTime -= EduTime.DeltaTime;

            if (_waitTime < 0)
            {
                _particleSystem.SetEmissionState(true);
            }
        }
    }
}

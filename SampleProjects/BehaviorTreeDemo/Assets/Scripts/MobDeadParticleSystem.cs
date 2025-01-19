using EduEngine;
using System.Numerics;

namespace BehaviorTreeDemo
{
    public class MobDeadParticleSystem : Component
    {
        private ParticleSystem _particleSystem;
        private float _timer = 0.0f;

        public MobDeadParticleSystem(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            _particleSystem = GameObject.GetComponent<ParticleSystem>();
        }

        public override void Update()
        {
            _timer -= EduTime.DeltaTime;

            if (_timer <= 0.0f)
                _particleSystem.SetEmissionState(false);

            if (_timer <= -2.0f)
            {
                GameObject.Destroy();
                _timer = float.MaxValue;
            }
        }

        public void Initialize(Texture colorTexture)
        {
            var data = new ParticleData();
            data.MinStartSize = Vector3.One * 0.4f;
            data.MaxStartSize = Vector3.One * 0.4f;
            data.EndSize = Vector3.One * 0.4f;
            data.MinStartColor = new Vector4(1, 1, 1, 1);
            data.MaxStartColor = new Vector4(1, 1, 1, 1);
            data.EndColor = new Vector4(1, 1, 1, 0);
            data.EmissionRate = 1000.0f;
            data.LifeTime = 2.0f;
            data.MaxParticles = 8;
            data.MinStartVelocity = new Vector3(-0.5f, 0.0f, -0.5f);
            data.MaxStartVelocity = new Vector3(0.5f, 1.0f, 0.5f);

            _particleSystem.SetStaticParticleData(data);
            _particleSystem.SetColorTexture(colorTexture);
            _particleSystem.SetEmissionState(true);
            _timer = 1.0f;
        }
    }
}

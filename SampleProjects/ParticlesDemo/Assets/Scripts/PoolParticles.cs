using EduEngine;
using System.Numerics;

namespace ParticlesDemo
{
    internal class PoolParticles : Component
    {
        [SerializeField] private float _maxOffset = 2;

        private Vector3 _startPos;
        private Vector3 _targetPos;
        private float _time = 0.0f;
        private float _moveTime = 1.0f;

        public PoolParticles(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            SetTargetPos();
        }

        public override void Update()
        {
            _time += EduTime.DeltaTime / _moveTime;
            GameObject.Transform.LocalPosition = Vector3.Lerp(_startPos, _targetPos, _time);

            if (_time >= 1.0f)
            {
                SetTargetPos();
                _time = 0.0f;
            }
        }

        private void SetTargetPos()
        {
            var x = (Random.Shared.NextSingle() * 2 - 1) * _maxOffset;
            var y = GameObject.Transform.LocalPosition.Y;
            var z = (Random.Shared.NextSingle() * 2 - 1) * _maxOffset;

            _startPos = GameObject.Transform.LocalPosition;
            _targetPos = new Vector3(x, y, z);

            _moveTime = 0.2f + Random.Shared.NextSingle() * 0.8f;
        }
    }
}

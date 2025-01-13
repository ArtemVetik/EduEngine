using EduEngine;
using System.Numerics;

namespace ParticlesDemo
{
    internal class TrailBallMove : Component
    {
        private Vector3 _startPos;
        private Vector3 _targetPos;
        private float _time = 0.0f;
        private float _moveTime = 1.0f;

        public TrailBallMove(GameObject parent)
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
            var x = (Random.Shared.NextSingle() * 2 - 1) * 3;
            var y = GameObject.Transform.LocalPosition.Y;
            var z = (Random.Shared.NextSingle() * 2 - 1) * 3;

            _startPos = GameObject.Transform.LocalPosition;
            _targetPos = new Vector3(x, y, z);

            _moveTime = 0.5f + Random.Shared.NextSingle() * 1.5f;
        }
    }
}

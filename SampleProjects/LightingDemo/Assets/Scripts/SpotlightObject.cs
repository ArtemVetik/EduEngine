using EduEngine;
using System.Numerics;

namespace LightingDemo
{
    public class SpotlightObject : Component
    {
        private float _rotationDuration = 2f;

        private Quaternion _startRotation;
        private Quaternion _targetRotation;
        private float _elapsedTime = 0.0f;

        public SpotlightObject(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            var light = GameObject.AddComponent<Light>();
            light.Type = LightType.Spotlight;
            light.Color = new Vector3(Random.Shared.NextSingle(), Random.Shared.NextSingle(), Random.Shared.NextSingle());
            light.Power = 20;
            light.FalloffEnd = 40.0f;
            light.SpotPower = 100.0f;

            _startRotation = GameObject.Transform.Rotation;
            _targetRotation = GenerateRandomRotation();
        }

        public override void Update()
        {
            _elapsedTime += EduTime.DeltaTime;

            float t = Math.Clamp(_elapsedTime / _rotationDuration, 0f, 1f);
            GameObject.Transform.Rotation = Quaternion.Slerp(_startRotation, _targetRotation, t);

            if (_elapsedTime >= _rotationDuration)
            {
                _elapsedTime = 0f;
                _rotationDuration = 1.0f + Random.Shared.NextSingle() * 2.0f;
                _startRotation = _targetRotation;
                _targetRotation = GenerateRandomRotation();
            }
        }

        private Quaternion GenerateRandomRotation()
        {
            var x = (Random.Shared.NextSingle() * 2 - 1) * MathF.PI / 2.0f;
            var z = (Random.Shared.NextSingle() * 2 - 1) * MathF.PI / 2.0f;
            return Quaternion.CreateFromYawPitchRoll(x, z, 0) * Quaternion.CreateFromAxisAngle(Vector3.UnitX, MathF.PI / 2.0f);
        }
    }
}

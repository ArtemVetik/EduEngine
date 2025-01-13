using EduEngine;
using System.Numerics;

namespace ParticlesDemo
{
    internal class TrailBallRoot : Component
    {
        private Quaternion _startRotation;
        private Quaternion _targetRotation;
        private float _time;

        public TrailBallRoot(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            SetTargetRotation();
        }

        public override void Update()
        {
            _time += EduTime.DeltaTime;

            GameObject.Transform.Rotation = Quaternion.Lerp(_startRotation, _targetRotation, _time);
            
            if (_time >= 1.0f)
                SetTargetRotation();
        }

        private void SetTargetRotation()
        {
            _startRotation = GameObject.Transform.Rotation;

            _targetRotation = GenerateRandomQuaternion();
            _time = 0.0f;
        }

        private static Quaternion GenerateRandomQuaternion()
        {
            Random random = new Random();

            double u1 = random.NextDouble();
            double u2 = random.NextDouble();
            double u3 = random.NextDouble();

            double sqrt1MinusU1 = Math.Sqrt(1 - u1);
            double sqrtU1 = Math.Sqrt(u1);

            double x = sqrt1MinusU1 * Math.Sin(2 * Math.PI * u2);
            double y = sqrt1MinusU1 * Math.Cos(2 * Math.PI * u2);
            double z = sqrtU1 * Math.Sin(2 * Math.PI * u3);
            double w = sqrtU1 * Math.Cos(2 * Math.PI * u3);

            return new Quaternion((float)x, (float)y, (float)z, (float)w);
        }
    }
}

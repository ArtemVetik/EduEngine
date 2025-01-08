using EduEngine;
using System.Numerics;

namespace LightingDemo
{
    public class PointLightObject : Component
    {
        private Vector3 _startPosition;
        private Vector3 _targetPosition;

        public PointLightObject(GameObject parent)
            : base(parent)
        {
        }

        public override void OnCreate()
        {
            _startPosition = GameObject.Transform.Position;
            _targetPosition = _startPosition;

            var light = GameObject.AddComponent<Light>();
            light.Type = LightType.Point;
            light.Color = new Vector3(Random.Shared.NextSingle(), Random.Shared.NextSingle(), Random.Shared.NextSingle());
            light.Power = 7;
            light.FalloffEnd = 10.0f;
        }

        public override void Update()
        {
            if (GameObject.Transform.Position == _targetPosition)
            {
                var dx = Random.Shared.Next(-10, 10);
                var dz = Random.Shared.Next(-10, 10);
                _targetPosition = _startPosition + new Vector3(dx, 0, dz);
            }

            GameObject.Transform.Position = MoveTowards(GameObject.Transform.Position, _targetPosition, 10.0f * EduTime.DeltaTime);
        }

        public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta)
        {
            Vector3 direction = target - current;
            float distance = direction.Length();

            if (distance <= maxDistanceDelta || distance == 0f)
                return target;

            return current + direction / distance * maxDistanceDelta;
        }
    }
}

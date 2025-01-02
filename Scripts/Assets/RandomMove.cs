using System.Numerics;

namespace EduEngine.Scripts
{
    public class RandomMove : Component
    {
        [SerializeField] private float _moveSpeed = 5.0f;

        private Vector3 _targetPosition;

        public RandomMove(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            UpdateTargetPosition();
        }

        public override void Update()
        {
            if (GameObject.Transform.Position == _targetPosition)
                UpdateTargetPosition();

            GameObject.Transform.Position = MoveTowards(GameObject.Transform.Position, _targetPosition, _moveSpeed * EduTime.DeltaTime);
        }

        private void UpdateTargetPosition()
        {
            _targetPosition = new Vector3(Random.Shared.Next(-30, 30), Random.Shared.Next(0, 30), Random.Shared.Next(-30, 30));
        }

        private static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta)
        {
            Vector3 direction = target - current;
            float distance = direction.Length();

            if (distance <= maxDistanceDelta || distance == 0f)
            {
                return target;
            }

            return current + direction / distance * maxDistanceDelta;
        }
    }
}

using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    public class CameraFollow : Component
    {
        [SerializeField] private GameObject _target;
        [SerializeField] private Vector3 _positionOffset;
        [SerializeField] private float _lookOffset;
        [SerializeField] private float _lerpForce = 50.0f;

        public CameraFollow(GameObject parent)
            : base(parent)
        { }

        public override void PhysicsUpdate()
        {
            var targetPosition = _target.Transform.Position - 
                                            _target.Transform.Forward * _positionOffset.Z +
                                            Vector3.UnitY * _positionOffset.Y +
                                            _target.Transform.Right * _positionOffset.X;

            var targetRotation = CreateLookRotation(_target.Transform.Forward + Vector3.UnitY * _lookOffset, Vector3.UnitY);

            GameObject.Transform.Position = Vector3.Lerp(GameObject.Transform.Position, targetPosition, _lerpForce * EduTime.DeltaTime);
            GameObject.Transform.Rotation = Quaternion.Lerp(GameObject.Transform.Rotation, targetRotation, _lerpForce * EduTime.DeltaTime);
        }

        private Quaternion CreateLookRotation(Vector3 forward, Vector3 up)
        {
            Vector3 right = Vector3.Normalize(Vector3.Cross(up, forward));
            up = Vector3.Normalize(Vector3.Cross(forward, right));

            Matrix4x4 rotationMatrix = new Matrix4x4(
                right.X, right.Y, right.Z, 0,
                up.X, up.Y, up.Z, 0,
                forward.X, forward.Y, forward.Z, 0,
                0, 0, 0, 1
            );

            return Quaternion.CreateFromRotationMatrix(rotationMatrix);
        }
    }
}

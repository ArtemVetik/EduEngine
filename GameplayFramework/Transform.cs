using System.Numerics;

namespace EduEngine
{
    public class Transform : Component
    {
        private Vector3 _position;
        private Quaternion _rotation;
        private Vector3 _localScale;
        private bool _isDirty;

        public Transform(GameObject parent)
            : base(parent)
        {
            Position = Vector3.Zero;
            Rotation = Quaternion.Identity;
            LocalScale = Vector3.One;

            UpdateWorldMatrix();
            _isDirty = false;
        }

        public Vector3 Position
        {
            get => _position;
            set
            {
                _position = value;
                _isDirty = true;
            }
        }
        public Quaternion Rotation
        {
            get => _rotation;
            set
            {
                _rotation = value;
                _isDirty = true;
            }
        }
        public Vector3 LocalScale
        {
            get => _localScale;
            set
            {
                _localScale = value;
                _isDirty = true;
            }
        }
        public Vector3 Forward => Vector3.Transform(Vector3.UnitZ, Rotation);
        public Vector3 Right => Vector3.Transform(Vector3.UnitX, Rotation);
        public Vector3 Up => Vector3.Transform(Vector3.UnitY, Rotation);
        public Matrix4x4 WorldMatrix { get; private set; }

        public override void Update()
        {
            if (_isDirty == false)
                return;

            UpdateWorldMatrix();
        }

        public void SetWorldMatrix(Matrix4x4 matrix)
        {
            WorldMatrix = matrix;

            Matrix4x4.Decompose(matrix, out Vector3 scale, out Quaternion rotation, out Vector3 translation);
            Position = translation;
            Rotation = rotation;
            LocalScale = scale;
        }

        private void UpdateWorldMatrix()
        {
            WorldMatrix = Matrix4x4.CreateScale(LocalScale) *
                Matrix4x4.CreateFromQuaternion(Rotation) *
                Matrix4x4.CreateTranslation(Position);
        }
    }
}

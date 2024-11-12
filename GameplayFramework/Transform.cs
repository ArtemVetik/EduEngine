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
        public Matrix4x4 WorldMatrix { get; private set; }

        public override void Update()
        {
            if (_isDirty == false)
                return;

            UpdateWorldMatrix();
        }

        private void UpdateWorldMatrix()
        {
            WorldMatrix = Matrix4x4.CreateScale(LocalScale) *
                Matrix4x4.CreateFromQuaternion(Rotation) *
                Matrix4x4.CreateTranslation(Position);
        }
    }
}

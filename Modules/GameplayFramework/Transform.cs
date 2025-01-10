using System.Numerics;

namespace EduEngine
{
    public class Transform : Component
    {
        private Vector3 _position;
        private Quaternion _rotation;
        private Vector3 _localScale;

        public Transform(GameObject parent)
            : base(parent)
        {
            LocalPosition = Vector3.Zero;
            LocalRotation = Quaternion.Identity;
            LocalScale = Vector3.One;

            UpdateWorldMatrix();
        }

        internal bool DirtyScale { get; private set; }

        public Vector3 Position
        {
            get => new Vector3(WorldMatrix.M41, WorldMatrix.M42, WorldMatrix.M43);
            set
            {
                if (GameObject.Parent == null)
                {
                    LocalPosition = value;
                }
                else
                {
                    Matrix4x4.Invert(GameObject.Parent.Transform.WorldMatrix, out Matrix4x4 inverseParentMatrix);
                    LocalPosition = Vector3.Transform(value, inverseParentMatrix);
                }
            }
        }
        public Vector3 LocalPosition
        {
            get => _position;
            set
            {
                _position = value;
                UpdateWorldMatrix();
            }
        }
        public Quaternion Rotation
        {
            get
            {
                Matrix4x4.Decompose(WorldMatrix, out _, out Quaternion rotation, out _);
                return rotation;
            }
            set
            {
                if (GameObject.Parent == null)
                {
                    LocalRotation = value;
                }
                else
                {
                    Matrix4x4.Invert(GameObject.Parent.Transform.WorldMatrix, out Matrix4x4 inverseParentMatrix);
                    Matrix4x4 globalRotationMatrix = Matrix4x4.CreateFromQuaternion(value);
                    Matrix4x4 localRotationMatrix = globalRotationMatrix * inverseParentMatrix;
                    Matrix4x4.Decompose(localRotationMatrix, out _, out Quaternion localRotation, out _);
                    LocalRotation = localRotation;
                }
            }
        }
        public Quaternion LocalRotation
        {
            get => _rotation;
            set
            {
                _rotation = value;
                UpdateWorldMatrix();
            }
        }
        public Vector3 LossyScale
        {
            get
            {
                Matrix4x4.Decompose(WorldMatrix, out Vector3 scale, out _, out _);
                return scale;
            }
        }
        public Vector3 LocalScale
        {
            get => _localScale;
            set
            {
                _localScale = value;
                UpdateWorldMatrix();
                DirtyScale = true;
            }
        }
        public Vector3 Forward => Vector3.Transform(Vector3.UnitZ, Rotation);
        public Vector3 Right => Vector3.Transform(Vector3.UnitX, Rotation);
        public Vector3 Up => Vector3.Transform(Vector3.UnitY, Rotation);
        public Matrix4x4 LocalWorldMatrix { get; private set; }
        public Matrix4x4 WorldMatrix => GameObject.Parent == null ? LocalWorldMatrix : LocalWorldMatrix * GameObject.Parent.Transform.WorldMatrix;

        public override void Update()
        {
            DirtyScale = false;
        }

        public override void UpdateEditor()
        {
            DirtyScale = false;
        }

        public void SetLocalWorldMatrix(Matrix4x4 matrix)
        {
            LocalWorldMatrix = matrix;

            Matrix4x4.Decompose(matrix, out Vector3 scale, out Quaternion rotation, out Vector3 translation);
            LocalPosition = translation;
            LocalRotation = rotation;
            LocalScale = scale;
        }

        private void UpdateWorldMatrix()
        {
            LocalWorldMatrix = Matrix4x4.CreateScale(LocalScale) *
                Matrix4x4.CreateFromQuaternion(LocalRotation) *
                Matrix4x4.CreateTranslation(LocalPosition);
        }
    }
}

using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public class RigidBody : Component, IDisposable
    {
        [SerializeField] private bool _isStatic = false;
        [SerializeField] private float _mass = 1.0f;

        private NativePhysicsObjectWrapper _physicObject;

        private List<Collider> _attachedColliders = new();

        public RigidBody(GameObject parent)
            : base(parent)
        {
        }

        public Vector3 Velocity => _physicObject.GetLinearVelocity();

        public bool IsStatic
        {
            get => _isStatic;
            set
            {
                if (_physicObject == null)
                    return;

                _isStatic = value;
                _physicObject.Dispose();
                _physicObject = new NativePhysicsObjectWrapper(GameObject.Transform.Position, GameObject.Transform.Rotation, IsStatic);
                AttachAllColliders();
            }
        }

        public override void OnAddComponent()
        {
            _physicObject = new NativePhysicsObjectWrapper(GameObject.Transform.Position, GameObject.Transform.Rotation, IsStatic);
            AttachAllColliders();
        }

        public void Dispose()
        {
            foreach (var collider in _attachedColliders)
                collider.ParentBody = null;

            _attachedColliders.Clear();

            _physicObject.Dispose();
            _physicObject = null;
        }

        public override void Update()
        {
            if (_physicObject == null)
                return;

            GameObject.Transform.Position = _physicObject.GetPosition();
            GameObject.Transform.Rotation = _physicObject.GetRotation();
        }

        public void SetTransform(Vector3 position, Quaternion rotation)
        {
            _physicObject.SetGlobalTransform(position, rotation);
        }

        public void AddForce(Vector3 force, ForceMode forceMode)
        {
            _physicObject?.AddForce(force, forceMode);
        }

        internal void AttachCollider(Collider collider)
        {
            _physicObject?.AttachShape(collider.GetShape());
            _attachedColliders.Add(collider);
        }

        internal void DetachCollider(Collider collider)
        {
            _physicObject?.DetachShape(collider.GetShape());
            _attachedColliders.Remove(collider);
        }

        private void AttachAllColliders()
        {
            foreach (var collider in _attachedColliders)
                collider.ParentBody = null;

            _attachedColliders.Clear();

            var colliders = GameObject.GetComponentsInChildren<Collider>();

            foreach (Collider collider in colliders)
            {
                if (collider.ParentBody != null)
                    continue;

                AttachCollider(collider);
                collider.ParentBody = this;
            }
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            if (fieldName == nameof(_isStatic))
                IsStatic = _isStatic;
            if (fieldName == nameof(_mass))
                _physicObject.SetMass(_mass);
        }
    }
}

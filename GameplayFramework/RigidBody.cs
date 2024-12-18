using System.Numerics;

namespace EduEngine
{
    public class RigidBody : Component, IDisposable
    {
        [SerializeField] private bool _isStatic = false;

        private NativePhysicsObjectWrapper _physicObject;

        public RigidBody(GameObject parent)
            : base(parent)
        {
        }

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

        public void AddForce(Vector3 force, ForceMode forceMode)
        {
            _physicObject?.AddForce(force, forceMode);
        }

        private void AttachAllColliders()
        {
            var colliders = GameObject.GetComponents<Collider>();

            foreach (Collider collider in colliders)
                _physicObject?.AttachShape(collider.GetShape());
        }
    }
}

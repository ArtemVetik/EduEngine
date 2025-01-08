using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public abstract class Collider : Component, IDisposable
    {
        [SerializeField] private bool _isTrigger = false;

        private NativePhysicsShapeWrapper _nativeShape;

        internal Collider(GameObject parent, ColliderData shape)
            : base(parent)
        {
            _nativeShape = new NativePhysicsShapeWrapper(shape, this);
            _nativeShape.SetTriggerEnterCallback(OnTriggerEnter);
            _nativeShape.SetTriggerExitCallback(OnTriggerExit);
            _nativeShape.SetCollisionEnterCallback(OnCollisionEnter);
            _nativeShape.SetCollisionExitCallback(OnCollisionExit);
        }

        public override void OnAddComponent()
        {
            SetTrigger(_isTrigger);
            var rigidbodyList = GameObject.GetComponentsInParent<RigidBody>();

            foreach (var body in rigidbodyList)
                body.AttachCollider(this);
        }

        public override void OnDestroy()
        {
            var rigidbodyList = GameObject.GetComponentsInParent<RigidBody>();

            foreach (var body in rigidbodyList)
                body.DetachCollider(this);
        }

        public void Dispose()
        {
            _nativeShape.Dispose();
            _nativeShape = null;
        }

        public void SetTrigger(bool isTrigger)
        {
            _nativeShape?.SetTrigger(isTrigger);
        }

        public override void Update()
        {
            _nativeShape?.SetLocalTransform(GameObject.Transform.LocalPosition, GameObject.Transform.LocalRotation);
            _nativeShape?.DebugDraw(Matrix4x4.CreateFromQuaternion(GameObject.Transform.Rotation) *
                                    Matrix4x4.CreateTranslation(GameObject.Transform.Position));
        }

        public override void UpdateEditor()
        {
            _nativeShape?.DebugDraw(Matrix4x4.CreateFromQuaternion(GameObject.Transform.Rotation) *
                                    Matrix4x4.CreateTranslation(GameObject.Transform.Position));
        }

        internal NativePhysicsShapeWrapper GetShape()
        {
            return _nativeShape;
        }

        internal void SetShape(ColliderData shape)
        {
            _nativeShape?.SetGeometry(shape);
        }

        private void OnTriggerEnter(object other)
        {
            var otherCollider = (Collider)other;
            var components = GameObject.GetComponents<Component>();

            foreach (var component in components)
                (component as IColliderCallbacks)?.OnTriggerEnter(otherCollider);
        }

        private void OnTriggerExit(object other)
        {
            var otherCollider = (Collider)other;
            var components = GameObject.GetComponents<Component>();

            foreach (var component in components)
                (component as IColliderCallbacks)?.OnTriggerExit(otherCollider);
        }

        private void OnCollisionEnter(CollisionData collisionData)
        {
            var components = GameObject.GetComponents<Component>();
            var a = collisionData.Contacts;
            foreach (var component in components)
                (component as IColliderCallbacks)?.OnCollisionEnter(collisionData);
        }

        private void OnCollisionExit(CollisionData collisionData)
        {
            var components = GameObject.GetComponents<Component>();

            foreach (var component in components)
                (component as IColliderCallbacks)?.OnCollisionExit(collisionData);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            SetTrigger(_isTrigger);
        }
    }
}

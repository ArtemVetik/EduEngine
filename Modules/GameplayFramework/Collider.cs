using System.Diagnostics.CodeAnalysis;

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
        }

        public override void OnAddComponent()
        {
            SetTrigger(_isTrigger);
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
            _nativeShape?.DebugDraw(GameObject.Transform.WorldMatrix);
        }

        public override void UpdateEditor()
        {
            _nativeShape?.DebugDraw(GameObject.Transform.WorldMatrix);
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

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            SetTrigger(_isTrigger);
        }
    }
}

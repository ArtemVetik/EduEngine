using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public abstract class Collider : Component, IDisposable
    {
        [SerializeField] private bool _isTrigger = false;
        [SerializeField] private float _staticFriction = 0.5f;
        [SerializeField] private float _dynamicFriction = 0.5f;
        [SerializeField] private float _restitution = 0.5f;

        private readonly List<Collider> _onTriggerEnter = new();
        private readonly List<Collider> _onTriggerExit = new();
        private readonly List<CollisionData> _onCollisionEnter = new();
        private readonly List<CollisionData> _onCollisionExit = new();

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

        public RigidBody ParentBody { get; internal set; } = null;

        public override void OnAddComponent()
        {
            SetTrigger(_isTrigger);
            SetMaterial(_staticFriction, _dynamicFriction, _restitution);
            var rigidbodyList = GameObject.GetComponentsInParent<RigidBody>();

            if (rigidbodyList.Length > 0)
            {
                rigidbodyList[0].AttachCollider(this);
                ParentBody = rigidbodyList[0];
            }
        }

        public void Dispose()
        {
            ParentBody?.DetachCollider(this);
            ParentBody = null;

            _nativeShape.Dispose();
            _nativeShape = null;
        }

        public void SetTrigger(bool isTrigger)
        {
            _nativeShape?.SetTrigger(isTrigger);
        }

        public void SetMaterial(float staticFriction, float dynamicFriction, float restitution)
        {
            _nativeShape?.SetMaterial(staticFriction, dynamicFriction, restitution);
        }

        public override void Update()
        {
            if (ParentBody != null && ParentBody.GameObject.Parent != GameObject.Parent)
                _nativeShape?.SetLocalTransform(GameObject.Transform.LocalPosition, GameObject.Transform.LocalRotation);

            if (_onTriggerEnter.Count > 0 || _onTriggerExit.Count > 0 || _onCollisionEnter.Count > 0 || _onCollisionExit.Count > 0)
            {
                var components = GameObject.GetComponents<Component>();

                foreach (var component in components)
                {
                    foreach (var item in _onTriggerEnter)
                        (component as IColliderCallbacks)?.OnTriggerEnter(item);
                    foreach (var item in _onTriggerExit)
                        (component as IColliderCallbacks)?.OnTriggerExit(item);
                    foreach (var item in _onCollisionEnter)
                        (component as IColliderCallbacks)?.OnCollisionEnter(item);
                    foreach (var item in _onCollisionExit)
                        (component as IColliderCallbacks)?.OnCollisionExit(item);
                }

                _onTriggerEnter.Clear();
                _onTriggerExit.Clear();
                _onCollisionEnter.Clear();
                _onCollisionExit.Clear();
            }
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
            _onTriggerEnter.Add(otherCollider);
        }

        private void OnTriggerExit(object other)
        {
            var otherCollider = (Collider)other;
            _onTriggerExit.Add(otherCollider);
        }

        private void OnCollisionEnter(CollisionData collisionData)
        {
            _onCollisionEnter.Add(collisionData);
        }

        private void OnCollisionExit(CollisionData collisionData)
        {
            _onCollisionExit.Add(collisionData);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            SetTrigger(_isTrigger);
            SetMaterial(_staticFriction, _dynamicFriction, _restitution);
        }
    }
}

namespace EduEngine
{
    public abstract class Collider : Component, IDisposable
    {
        private NativePhysicsShapeWrapper _nativeShape;

        internal Collider(GameObject parent, ColliderData shape)
            : base(parent)
        {
            _nativeShape =  new NativePhysicsShapeWrapper(shape);
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
    }
}

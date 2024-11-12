namespace EduEngine
{
    public abstract class Collider : Component, IDisposable
    {
        private NativePhysicsShapeWrapper _nativeShape;

        public Collider(GameObject parent, ColliderData shape)
            : base(parent)
        {
            _nativeShape = PhysicsWorldInterop.CreateShape(shape);
        }

        public void Dispose()
        {
            _nativeShape.Dispose();
        }

        public override void Update()
        {
            _nativeShape.DebugDraw(Parent.Transform.WorldMatrix);
        }

        internal NativePhysicsShapeWrapper GetShape()
        {
            return _nativeShape;
        }

        protected void SetShape(ColliderData shape)
        {
            _nativeShape.SetGeometry(shape);
        }
    }
}

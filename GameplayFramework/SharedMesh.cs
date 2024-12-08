namespace EduEngine
{
    public class SharedMesh : Asset
    {
        private SharedMeshWrapper _nativeMeshWrapper;

        public SharedMesh(string guid, string assetPath)
            : base(guid)
        {
            _nativeMeshWrapper = new SharedMeshWrapper(assetPath);
        }

        public int VertexCount => _nativeMeshWrapper.GetVertexCount();
        public int IndexCount => _nativeMeshWrapper.GetIndexCount();

        internal SharedMeshWrapper GetWrapper() => _nativeMeshWrapper;

        protected override void OnDelete()
        {
            _nativeMeshWrapper.Dispose();
        }
    }
}

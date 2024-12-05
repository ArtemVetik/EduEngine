namespace EduEngine
{
    public class SharedMesh
    {
        private SharedMeshWrapper _nativeMeshWrapper;

        public SharedMesh(string assetPath)
        {
            _nativeMeshWrapper = new SharedMeshWrapper(assetPath);
        }

        internal SharedMeshWrapper GetWrapper() => _nativeMeshWrapper;
    }
}

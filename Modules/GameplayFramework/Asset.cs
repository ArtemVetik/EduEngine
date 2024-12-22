namespace EduEngine
{
    public abstract class Asset
    {
        private string _guid;

        public Asset(string guid)
        {
            _guid = guid;
        }

        public string GUID => _guid;
        public bool IsValid => !string.IsNullOrEmpty(_guid);

        internal void UpdatePath(string globalPath)
        {
            OnPathUpdated(globalPath);
        }

        internal void Delete()
        {
            _guid = string.Empty;
            OnDelete();
        }

        protected abstract void OnDelete();
        protected abstract void OnPathUpdated(string globalPath);
    }
}

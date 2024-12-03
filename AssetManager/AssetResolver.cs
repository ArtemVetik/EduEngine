using Newtonsoft.Json;

namespace EduEngine
{
    internal class AssetResolver : IDisposable
    {
        private readonly string _path;
        private readonly FileSystemWatcher _fileSystemWatcher;

        internal AssetResolver(string rootPath)
        {
            if (Directory.Exists(rootPath) == false)
                throw new ArgumentException(nameof(rootPath));

            _path = rootPath;
            HasChanges = false;

            _fileSystemWatcher = new FileSystemWatcher(rootPath);
            _fileSystemWatcher.IncludeSubdirectories = true;
            _fileSystemWatcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.LastWrite | NotifyFilters.DirectoryName;

            _fileSystemWatcher.Created += OnCreated;
            _fileSystemWatcher.Deleted += OnDeleted;
            _fileSystemWatcher.Changed += OnChanged;
            _fileSystemWatcher.Renamed += OnRenamed;

            _fileSystemWatcher.Filter = "*.*";

            _fileSystemWatcher.EnableRaisingEvents = true;
        }

        public string RootPath => _path;
        public bool HasChanges { get; private set; }

        public bool IsPathInside(string filePath)
        {
            string fullFilePath = Path.GetFullPath(filePath).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
            string fullFolderPath = Path.GetFullPath(_path).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);

            return fullFilePath.StartsWith(fullFolderPath, StringComparison.OrdinalIgnoreCase);
        }

        public Dictionary<string, string> Resolve()
        {
            _fileSystemWatcher.EnableRaisingEvents = false;

            Dictionary<string, string> resources = new();

            foreach (var file in Directory.EnumerateFiles(_path, "*.*", SearchOption.AllDirectories))
            {
                if (IsTargetFile(file))
                {
                    ProcessTargetFile(file, ref resources);
                }
                else if (IsMetaFile(file))
                {
                    ProcessMetaFile(file, ref resources);
                }
                else if (IsFolder(file))
                {
                    ProcessFolder(file, ref resources);
                }
            }

            HasChanges = false;
            _fileSystemWatcher.EnableRaisingEvents = true;
            return resources;
        }

        private void ProcessTargetFile(string file, ref Dictionary<string, string> resources)
        {
            var metaFile = file + ".meta";
            if (File.Exists(metaFile))
            {
                var metaData = JsonConvert.DeserializeObject<AssetMetaData>(File.ReadAllText(metaFile));

                if (IsValidMetaFile(metaData, file, false, ref resources) == false)
                {
                    metaData = AssetMetaData.NewData(false);
                    File.WriteAllText(metaFile, JsonConvert.SerializeObject(metaData));
                }

                resources[metaData.GUID] = ToLocalPath(file);
            }
            else
            {
                var meta = AssetMetaData.NewData(false);
                File.WriteAllText(metaFile, JsonConvert.SerializeObject(meta));
                resources[meta.GUID] = ToLocalPath(file);
            }
        }

        private void ProcessMetaFile(string file, ref Dictionary<string, string> resources)
        {
            var metaData = JsonConvert.DeserializeObject<AssetMetaData>(File.ReadAllText(file));

            if (metaData == null)
            {
                File.Delete(file);
                return;
            }

            var targetFile = Path.Combine(
                Path.GetDirectoryName(file) ?? string.Empty,
                Path.GetFileNameWithoutExtension(file)
            );

            if (IsValidMetaFile(metaData, targetFile, IsFolder(targetFile), ref resources) == false)
            {
                metaData = AssetMetaData.NewData(false);
                File.WriteAllText(file, JsonConvert.SerializeObject(metaData));
            }

            if (File.Exists(targetFile))
            {
                resources[metaData.GUID] = ToLocalPath(targetFile);
            }
            else
            {
                File.Delete(file);
            }
        }

        private void ProcessFolder(string file, ref Dictionary<string, string> resources)
        {
            var metaFile = file + ".meta";
            if (File.Exists(metaFile))
            {
                var metaData = JsonConvert.DeserializeObject<AssetMetaData>(File.ReadAllText(metaFile));

                if (IsValidMetaFile(metaData, file, true, ref resources) == false)
                {
                    metaData = AssetMetaData.NewData(true);
                    File.WriteAllText(metaFile, JsonConvert.SerializeObject(metaData));
                }

                resources.Add(metaData.GUID, ToLocalPath(file));
            }
            else
            {
                var meta = AssetMetaData.NewData(true);
                File.WriteAllText(metaFile, JsonConvert.SerializeObject(meta));
                resources.Add(meta.GUID, ToLocalPath(file));
            }
        }

        private bool IsValidMetaFile(AssetMetaData? metaData, string targetFile, bool isFolder, ref Dictionary<string, string> resources)
        {
            return metaData != null && metaData.IsFolder == isFolder &&
                (resources.TryGetValue(metaData.GUID, out string? outFile) == false || outFile == ToLocalPath(targetFile));
        }

        private static bool IsFolder(string path)
        {
            return Directory.Exists(path);
        }

        private static bool IsTargetFile(string filePath)
        {
            string extension = Path.GetExtension(filePath);

            if (extension == null)
                return false;

            return extension.ToLower() is ".scene" or ".cs" or ".fbx";
        }

        private static bool IsMetaFile(string filePath)
        {
            return IsTargetFile(Path.GetFileNameWithoutExtension(filePath));
        }

        private void OnRenamed(object sender, RenamedEventArgs e)
        {
            if (IsTargetFile(e.FullPath) || IsMetaFile(e.FullPath) || IsFolder(e.FullPath))
                HasChanges = true;
        }

        private void OnChanged(object sender, FileSystemEventArgs e)
        {
            if (IsTargetFile(e.FullPath) || IsMetaFile(e.FullPath) || IsFolder(e.FullPath))
                HasChanges = true;
        }

        private void OnDeleted(object sender, FileSystemEventArgs e)
        {
            if (IsTargetFile(e.FullPath) || IsMetaFile(e.FullPath) || IsFolder(e.FullPath))
                HasChanges = true;
        }

        private void OnCreated(object sender, FileSystemEventArgs e)
        {
            if (IsTargetFile(e.FullPath) || IsMetaFile(e.FullPath) || IsFolder(e.FullPath))
                HasChanges = true;
        }

        private string ToLocalPath(string path)
        {
            if (path.StartsWith(_path))
                return path.Remove(0, _path.Length);

            return path;
        }

        public void Dispose()
        {
            _fileSystemWatcher.Dispose();
        }
    }
}

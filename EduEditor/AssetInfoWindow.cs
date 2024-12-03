using ImGuiNET;

namespace EduEngine.Editor
{
    internal class AssetInfoWindow
    {
        private string _guid = null;
        private AssetMetaData _metaData;
        private AssetType _assetType;
        private FileInfo _sourceInfo;

        public void Render(string guid)
        {
            if (guid != _guid)
            {
                _guid = guid;
                _metaData = AssetDataBase.GetMetaData(guid);
                _assetType = AssetDataBase.GetType(guid);

                var sourcePath = AssetDataBase.GetGlobalPathByGUID(guid);
                _sourceInfo = new FileInfo(sourcePath);

                ImGui.SetWindowFocus("AssetInfo");
            }

            ImGui.Begin("AssetInfo");

            if (_guid == null)
            {
                ImGui.End();
                return;
            }
            
            ImGui.Text("Type: " + _assetType.ToString());
            ImGui.Text("GUID: " + _metaData.GUID);
            if (_sourceInfo.Length < 1024)
                ImGui.Text($"Size: {_sourceInfo.Length:F2} byte");
            else if (_sourceInfo.Length < 1024 * 1024)
                ImGui.Text($"Size: {_sourceInfo.Length / 1024.0f:F2} Kb");
            else
                ImGui.Text($"Size: {_sourceInfo.Length / 1024.0f:F2} Mb");
            ImGui.Text($"Created: {_sourceInfo.CreationTime}");
            ImGui.Text($"Modified: {_sourceInfo.LastWriteTime}");

            ImGui.Separator();

            if (_assetType == AssetType.Scene)
            {
                if (EngineStateManager.CurrentState == EngineState.Runtime)
                {
                    ImGui.Text("Scene cannot be loaded in a runtime state");
                    ImGui.BeginDisabled();
                }

                if (ImGui.Button("Load Scene"))
                {
                    AssetDataBase.LoadScene(AssetDataBase.GetLocalPathByGUID(_guid), out SceneData? scene);
                    SceneImporter.LoadScene(scene, false);
                }

                if (EngineStateManager.CurrentState == EngineState.Runtime)
                    ImGui.EndDisabled();
            }

            ImGui.End();
        }
    }
}

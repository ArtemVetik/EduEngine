using ImGuiNET;
using System.Numerics;

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
            if (string.IsNullOrEmpty(guid))
                return;

            if (guid != _guid)
            {
                _guid = guid;
                _metaData = AssetDataBase.GetAssetData(guid).MetaData;
                _assetType = AssetDataBase.GetAssetData(guid).Type;

                ImGui.SetWindowFocus("AssetInfo");
            }

            if (_sourceInfo == null || _sourceInfo.Exists == false)
            {
                var sourcePath = AssetDataBase.GetAssetData(guid).GlobalPath;
                _sourceInfo = new FileInfo(sourcePath);
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
                RenderSceneInfo();
            else if (_assetType == AssetType.Mesh)
                RenderMeshInfo();
            else if (_assetType == AssetType.Texture)
                RenderTextureInfo();

            ImGui.End();
        }

        private void RenderSceneInfo()
        {
            if (EngineStateManager.CurrentState == EngineState.Runtime)
            {
                ImGui.Text("Scene cannot be loaded in a runtime state");
                ImGui.BeginDisabled();
            }

            if (ImGui.Button("Load Scene"))
            {
                SceneImporter.LoadScene(_guid, false);
            }

            if (EngineStateManager.CurrentState == EngineState.Runtime)
                ImGui.EndDisabled();
        }

        private void RenderMeshInfo()
        {
            var meshObject = AssetDataBase.HasGUID(_guid)
                ? AssetDataBase.AllAssets[_guid].Asset as SharedMesh
                : null;

            if (meshObject == null)
            {
                ImGui.Text("Invalid mesh object");
                return;
            }

            ImGui.Text($"Vertices: {meshObject.VertexCount}");
            ImGui.Text($"Indices: {meshObject.IndexCount}");
        }

        private void RenderTextureInfo()
        {
            var textureObject = AssetDataBase.HasGUID(_guid)
                ? AssetDataBase.AllAssets[_guid].Asset as Texture
                : null;

            if (textureObject == null)
            {
                EditorRenderEngineInterop.PreviewTexture(null);
                ImGui.Text("Invalid texture object");
                return;
            }

            var texPtr = EditorRenderEngineInterop.PreviewTexture(AssetDataBase.GetAssetData(_guid).GlobalPath);
            ImGui.Image(texPtr, new Vector2(200, 200));
        }
    }
}

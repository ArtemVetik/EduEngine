using ImGuiNET;
using System;
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

            TexturePreview.UpdatePreview(_guid);
            MeshPreview.UpdatePreview(_guid);

            if (_assetType == AssetType.Scene)
                RenderSceneInfo();
            else if (_assetType == AssetType.Mesh)
                RenderMeshInfo();
            else if (_assetType == AssetType.Texture)
                RenderTextureInfo();
            else if (_assetType == AssetType.Material)
                RenderMaterialInfo();

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

            ImGui.Text($"Vertices: {MeshPreview.Info.VertexCount}");
            ImGui.Text($"Indices: {MeshPreview.Info.IndexCount}");

            ImGui.Image(MeshPreview.Info.TexturePtr, new Vector2(200, 200));

            if (ImGui.IsWindowHovered())
            {
                if (Input.Editor.MouseButtonPressed(0))
                {
                    var delta = new Vector3(Input.Editor.MouseDelta.X, Input.Editor.MouseDelta.Y, Input.Editor.ScrollDelta.Y);
                    EditorRenderEngineInterop.RotatePreviewMesh(delta * EditorTime.DeltaTime);
                }
                else
                {
                    var delta = new Vector3(0, 0, Input.Editor.ScrollDelta.Y);
                    EditorRenderEngineInterop.RotatePreviewMesh(delta * EditorTime.DeltaTime);
                }
            }
        }

        private void RenderTextureInfo()
        {
            if (TexturePreview.TexturePtr == nint.Zero)
            {
                ImGui.Text("Invalid texture object");
                return;
            }

            ImGui.Image(TexturePreview.TexturePtr, new Vector2(200, 200));
        }

        private void RenderMaterialInfo()
        {
            var currentAsset = AssetDataBase.HasGUID(_guid) 
                ? AssetDataBase.GetAssetData(_guid).Asset as Material
                : null;

            if (currentAsset == null || currentAsset.IsValid == false)
            {
                ImGui.Text("Invalid material object");
                return;
            }

            if (currentAsset.MainTexture != null && currentAsset.MainTexture.IsValid == false)
            {
                currentAsset.SetMainTexture(null);
                MaterialImporter.SaveMaterial(currentAsset);
            }

            var assets = AssetDataBase.AllAssets.Where(asset => asset.Value.Asset != null &&
                                                                asset.Value.Asset.GetType() == typeof(Texture));

            if (ImGui.BeginCombo("MainTexture", currentAsset.MainTexture == null ? "null" : AssetDataBase.GetAssetData(currentAsset.MainTexture.GUID).LocalPath))
            {
                if (ImGui.Selectable("null"))
                {
                    currentAsset.SetMainTexture(null);
                    MaterialImporter.SaveMaterial(currentAsset);
                }

                foreach (var asset in assets)
                {
                    if (ImGui.Selectable(asset.Value.LocalPath))
                    {
                        currentAsset.SetMainTexture(asset.Value.Asset as Texture);
                        MaterialImporter.SaveMaterial(currentAsset);
                    }
                }
                ImGui.EndCombo();
            }
        }
    }
}

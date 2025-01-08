using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class AssetInfoWindow
    {
        private string _guid = null;
        private AssetData _assetData = null;
        private FileInfo _sourceInfo = null;

        public void Render(string guid)
        {
            if (guid == null || AssetDataBase.HasGUID(guid) == false)
            {
                ImGui.Begin("AssetInfo");
                ImGui.Text("Invalid asset!");
                ImGui.Text($"GUID: {guid}");
                ImGui.End();
                return;
            }

            if (guid != _guid)
            {
                _guid = guid;
                _assetData = AssetDataBase.GetAssetData(guid);
                _sourceInfo = null;

                ImGui.SetWindowFocus("AssetInfo");
            }

            if (_sourceInfo == null || _sourceInfo.Exists == false)
                _sourceInfo = new FileInfo(_assetData.GlobalPath);

            ImGui.Begin("AssetInfo");

            ImGui.Text("Type: " + _assetData.Type.ToString());
            ImGui.Text("GUID: " + _assetData.MetaData.GUID);
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

            switch (_assetData.Type)
            {
                case AssetType.Scene:
                    RenderSceneInfo();
                    break;
                case AssetType.Mesh:
                    RenderMeshInfo();
                    break;
                case AssetType.Texture:
                    RenderTextureInfo();
                    break;
                case AssetType.Material:
                    RenderMaterialInfo();
                    break;
            }

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
            var meshObject = AssetDataBase.AllAssets[_guid].Asset as SharedMesh;

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
                    var delta = new Vector3(Input.Editor.MouseDelta.X, Input.Editor.MouseDelta.Y, Input.Editor.ScrollDelta.Y) * 5.0f;
                    EditorRenderEngineInterop.RotatePreviewMesh(delta * EditorTime.DeltaTime);
                }
                else
                {
                    var delta = new Vector3(0, 0, Input.Editor.ScrollDelta.Y) * 5.0f;
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
            var currentAsset = AssetDataBase.GetAssetData(_guid).Asset as Material;

            if (currentAsset == null || currentAsset.IsValid == false)
            {
                ImGui.Text("Invalid material object");
                return;
            }

            bool isDirty = false;

            ImGuiEx.RenderAssetSelect(typeof(Texture), currentAsset.MainTexture, "MainTex", (selectedAsset) =>
            {
                currentAsset.SetMainTexture(selectedAsset as Texture);
                isDirty = true;
            });

            var diffuseAlbedo = currentAsset.DiffuseAlbedo;
            if (ImGui.ColorPicker4("Diffuse Albedo", ref diffuseAlbedo))
            {
                currentAsset.DiffuseAlbedo = diffuseAlbedo;
                isDirty = true;
            }

            var fresnelR0 = currentAsset.FresnelR0;
            if (ImGui.SliderFloat3("Fresnel R0", ref fresnelR0, 0.02f, 0.999f))
            {
                currentAsset.FresnelR0 = fresnelR0;
                isDirty = true;
            }

            var roughness = currentAsset.Roughness;
            if (ImGui.SliderFloat("Roughness", ref roughness, 0.0f, 0.999f))
            {
                currentAsset.Roughness = roughness;
                isDirty = true;
            }

            if (isDirty)
                MaterialImporter.SaveMaterial(currentAsset);
        }
    }
}

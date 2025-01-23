using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class AssetWindow
    {
        internal struct AssetPopupInfo
        {
            public string Guid;
            public bool Rename;
            public bool Delete;
        }

        private readonly Dictionary<AssetType, Vector4> _assetColors = new()
        {
            { AssetType.Scene, new Vector4(0.8f, 0.6f, 0.2f, 1.0f) },
            { AssetType.Script, new Vector4(0.56f, 0.75f, 0.43f, 1.0f) },
            { AssetType.Mesh, new Vector4(0.26f, 0.38f, 0.93f, 1.0f) },
            { AssetType.Texture, new Vector4(0.66f, 0.42f, 0.79f, 1.0f) },
            { AssetType.Material, new Vector4(0.35f, 0.42f, 0.62f, 1.0f) },
            { AssetType.Invalid, new Vector4(0.84f, 0.16f, 0.16f, 1.0f) },
        };

        private readonly string[] _filters = Enum.GetNames<AssetType>();

        private AssetType? _currentFilter = null;
        private string _popupInput = string.Empty;
        private float _buttonSize = 64.0f;
        private AssetPopupInfo _popupInfo;

        public string SelectedAsset { get; private set; } = null;

        public void Render()
        {
            ImGui.Begin("Asset Browser", ImGuiWindowFlags.MenuBar);

            RenderMenuBar(out bool createScene, out bool createMaterial);
            RenderScenePopup(createScene);
            RenderMaterialPopup(createMaterial);
            RenderFilter();

            ImGui.Separator();

            RenderAssets();
            RenderAssetsPopup();

            ImGui.End();
        }

        private void RenderMenuBar(out bool createScene, out bool createMaterial)
        {
            createScene = false;
            createMaterial = false;

            if (ImGui.BeginMenuBar())
            {
                if (ImGui.BeginMenu("Create"))
                {
                    if (ImGui.MenuItem("Empty Scene"))
                    {
                        createScene = true;
                    }
                    if (ImGui.MenuItem("New Material"))
                    {
                        createMaterial = true;
                    }
                    ImGui.EndMenu();
                }
                if (ImGui.BeginMenu("Options"))
                {
                    ImGui.SliderFloat("Cell Size", ref _buttonSize, 32.0f, 128.0f);
                    ImGui.EndMenu();
                }
                ImGui.EndMenuBar();
            }
        }

        private void RenderScenePopup(bool open)
        {
            if (open)
                ImGui.OpenPopup("CreateScenePopup");

            if (ImGui.BeginPopupModal("CreateScenePopup", ImGuiWindowFlags.AlwaysAutoResize))
            {
                ImGui.InputText("Enter scene name", ref _popupInput, 100);

                if (ImGui.Button("OK"))
                {
                    SceneImporter.CreateSceneFile(_popupInput, new SceneData());
                    _popupInput = string.Empty;
                    ImGui.CloseCurrentPopup();
                }

                ImGui.SameLine();

                if (ImGui.Button("Cancel"))
                {
                    _popupInput = string.Empty;
                    ImGui.CloseCurrentPopup();
                }

                ImGui.EndPopup();
            }
        }

        private void RenderMaterialPopup(bool open)
        {
            if (open)
                ImGui.OpenPopup("CreateMaterialPopup");

            if (ImGui.BeginPopupModal("CreateMaterialPopup", ImGuiWindowFlags.AlwaysAutoResize))
            {
                ImGui.InputText("Enter material name", ref _popupInput, 100);

                if (ImGui.Button("OK"))
                {
                    MaterialImporter.CreateMaterialFile(_popupInput, new MaterialData());
                    _popupInput = string.Empty;
                    ImGui.CloseCurrentPopup();
                }

                ImGui.SameLine();

                if (ImGui.Button("Cancel"))
                {
                    _popupInput = string.Empty;
                    ImGui.CloseCurrentPopup();
                }

                ImGui.EndPopup();
            }
        }

        private void RenderFilter()
        {
            ImGui.PushItemWidth(ImGui.GetContentRegionAvail().X / 4);
            if (ImGui.BeginCombo("Filter", _currentFilter == null ? "All" : _currentFilter.ToString()))
            {
                if (ImGui.Selectable("All"))
                {
                    _currentFilter = null;
                }

                foreach (var type in _filters)
                {
                    var assetType = Enum.Parse<AssetType>(type);
                    ImGui.PushStyleColor(ImGuiCol.Text, _assetColors[assetType]);
                    if (ImGui.Selectable(type))
                    {
                        _currentFilter = assetType;
                    }
                    ImGui.PopStyleColor();
                }
                ImGui.EndCombo();
            }
            ImGui.PopItemWidth();
        }

        private void RenderAssets()
        {
            float windowWidth = ImGui.GetContentRegionAvail().X;
            int columnCount = Math.Max(1, (int)(windowWidth / (_buttonSize + ImGui.GetStyle().ItemSpacing.X)));
            int columnIndex = 0;

            foreach (var asset in AssetDataBase.AllAssets)
            {
                var type = AssetDataBase.GetAssetData(asset.Key).Type;

                if (_currentFilter != null && type != _currentFilter)
                    continue;

                ImGui.PushStyleColor(ImGuiCol.Button, _assetColors[type] * (asset.Key == SelectedAsset ? 1.3f : 1.0f));
                ImGui.PushStyleColor(ImGuiCol.ButtonHovered, _assetColors[type] * 1.1f);
                ImGui.PushStyleColor(ImGuiCol.ButtonActive, _assetColors[type] * 0.9f);

                if (ImGui.Button(Path.GetFileNameWithoutExtension(asset.Value.LocalPath) + $"##{asset.Key}", new Vector2(_buttonSize, _buttonSize)))
                {
                    SelectedAsset = asset.Key;
                }

                if (ImGui.IsItemClicked(ImGuiMouseButton.Right) && EngineStateManager.CurrentState != EngineState.Runtime)
                {
                    ImGui.OpenPopup("AssetItem##" + asset.Key);
                }

                if (ImGui.BeginPopup("AssetItem##" + asset.Key))
                {
                    if (ImGui.MenuItem("Rename"))
                    {
                        _popupInfo.Guid = asset.Key;
                        _popupInfo.Rename = true;
                    }
                    if (ImGui.MenuItem("Delete"))
                    {
                        _popupInfo.Guid = asset.Key;
                        _popupInfo.Delete = true;
                    }
                    ImGui.EndPopup();
                }

                ImGui.PopStyleColor();
                ImGui.PopStyleColor();
                ImGui.PopStyleColor();

                if (ImGui.IsItemHovered())
                {
                    ImGui.SetTooltip(asset.Value.LocalPath);
                }

                columnIndex++;
                if (columnIndex >= columnCount)
                {
                    columnIndex = 0;
                    ImGui.NewLine();
                }
                else
                {
                    ImGui.SameLine();
                }
            }
        }

        private void RenderAssetsPopup()
        {
            if (EngineStateManager.CurrentState == EngineState.Runtime)
                return;

            if (_popupInfo.Delete)
            {
                ImGui.OpenPopup("Delete Asset");
                _popupInfo.Delete = false;
            }

            if (ImGui.BeginPopupModal("Delete Asset", ImGuiWindowFlags.AlwaysAutoResize))
            {
                var asset = AssetDataBase.GetAssetData(_popupInfo.Guid);

                ImGui.Text($"Delete Asset {asset.LocalPath}");

                if (ImGui.Button("Yes"))
                {
                    File.Delete(asset.GlobalPath);
                    AssetDataBase.Resolve();
                    ImGui.CloseCurrentPopup();
                }

                ImGui.SameLine();

                if (ImGui.Button("No"))
                {
                    ImGui.CloseCurrentPopup();
                }

                ImGui.EndPopup();
            }

            if (_popupInfo.Rename)
            {
                ImGui.OpenPopup("Rename Asset");
                _popupInfo.Rename = false;
            }

            if (ImGui.BeginPopupModal("Rename Asset", ImGuiWindowFlags.AlwaysAutoResize))
            {
                var asset = AssetDataBase.GetAssetData(_popupInfo.Guid);

                ImGui.InputText("New Name", ref _popupInput, 100);

                ImGui.Text($"Rename Asset");
                ImGui.Text($"From: {Path.GetFileName(asset.LocalPath)}");
                ImGui.Text($"To: {_popupInput}{Path.GetExtension(asset.LocalPath)}");

                if (ImGui.Button("Yes"))
                {
                    string directoryPath = Path.GetDirectoryName(asset.GlobalPath);
                    string newFilePath = Path.Combine(directoryPath, _popupInput) + Path.GetExtension(asset.LocalPath);

                    File.Move(asset.GlobalPath, newFilePath);
                    File.Move(asset.GlobalPath + ".meta", newFilePath + ".meta");
                    AssetDataBase.Resolve();
                    ImGui.CloseCurrentPopup();
                }

                ImGui.SameLine();

                if (ImGui.Button("No"))
                {
                    ImGui.CloseCurrentPopup();
                }

                ImGui.EndPopup();
            }
        }
    }
}

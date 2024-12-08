using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class AssetWindow
    {
        private readonly Dictionary<AssetType, Vector4> _assetColors = new()
        {
            { AssetType.Scene, new Vector4(0.8f, 0.6f, 0.2f, 1.0f) },
            { AssetType.Script, new Vector4(0.56f, 0.75f, 0.43f, 1.0f) },
            { AssetType.Mesh, new Vector4(0.26f, 0.38f, 0.93f, 1.0f) },
            { AssetType.Invalid, new Vector4(0.84f, 0.16f, 0.16f, 1.0f) },
        };

        private float _buttonSize = 64.0f;
        private string[] _filters = Enum.GetNames<AssetType>();
        private AssetType? _currentFilter = null;
        private string _popupInput = string.Empty;
        private string _selectedAsset = null;

        public string SelectedAsset => _selectedAsset;

        public void Render()
        {
            ImGui.Begin("Asset Browser", ImGuiWindowFlags.MenuBar);

            RenderMenuBar(out bool openScenePopup);

            if (openScenePopup)
                ImGui.OpenPopup("Create Scene");

            RenderScenePopup();
            RenderFilter();

            ImGui.Separator();

            RenderAssets();

            ImGui.End();
        }

        private void RenderMenuBar(out bool openScenePopup)
        {
            openScenePopup = false;

            if (ImGui.BeginMenuBar())
            {
                if (ImGui.BeginMenu("Create"))
                {
                    if (ImGui.MenuItem("Empty Scene"))
                    {
                        openScenePopup = true;
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

        private void RenderScenePopup()
        {
            if (ImGui.BeginPopupModal("Create Scene", ImGuiWindowFlags.AlwaysAutoResize))
            {
                ImGui.InputText("Enter scene name", ref _popupInput, 100);

                if (ImGui.Button("OK"))
                {
                    AssetDataBase.CreateScene(_popupInput, new SceneData());
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
            ImGui.PushItemWidth(ImGui.GetContentRegionAvail().X / 2);
            if (ImGui.BeginCombo("Filter", _currentFilter == null ? "All" : _currentFilter.ToString()))
            {
                if (ImGui.Selectable("All"))
                {
                    _currentFilter = null;
                }

                foreach (var type in _filters)
                {
                    var tyt = Enum.Parse<AssetType>(type);
                    ImGui.PushStyleColor(ImGuiCol.Text, _assetColors[tyt]);
                    if (ImGui.Selectable(type))
                    {
                        _currentFilter = tyt;
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
                var type = AssetDataBase.GetType(asset.Key);

                if (_currentFilter != null && type != _currentFilter)
                    continue;

                ImGui.PushStyleColor(ImGuiCol.Button, _assetColors[type] * (asset.Key == _selectedAsset ? 1.3f : 1.0f));
                ImGui.PushStyleColor(ImGuiCol.ButtonHovered, _assetColors[type] * 1.1f);
                ImGui.PushStyleColor(ImGuiCol.ButtonActive, _assetColors[type] * 0.9f);

                if (ImGui.Button(Path.GetFileNameWithoutExtension(asset.Value.LocalPath) + $"##{asset.Key}", new Vector2(_buttonSize, _buttonSize)))
                {
                    _selectedAsset = asset.Key;
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
    }
}

using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    public static class AssetWindow
    {
        private static readonly Dictionary<AssetType, Vector4> _assetColors = new()
        {
            { AssetType.Scene, new Vector4(0.8f, 0.6f, 0.2f, 1.0f) },
            { AssetType.Script, new Vector4(0.56f, 0.75f, 0.43f, 1.0f) },
            { AssetType.Mesh, new Vector4(0.26f, 0.38f, 0.93f, 1.0f) },
            { AssetType.Invalid, new Vector4(0.84f, 0.16f, 0.16f, 1.0f) },
        };

        private static float _buttonSize = 64.0f;
        private static string[] _filters = Enum.GetNames<AssetType>();
        private static AssetType? _currentFilter = null;

        public static void Render()
        {
            ImGui.Begin("Asset Browser");

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

            ImGui.SameLine();
            ImGui.PushItemWidth(ImGui.GetContentRegionAvail().X - ImGui.CalcTextSize("Cell Size").X);
            ImGui.SliderFloat("Cell Size", ref _buttonSize, 32.0f, 128.0f);
            ImGui.PopItemWidth();

            ImGui.Separator();

            float windowWidth = ImGui.GetContentRegionAvail().X;
            int columnCount = Math.Max(1, (int)(windowWidth / (_buttonSize + ImGui.GetStyle().ItemSpacing.X)));
            int columnIndex = 0;

            foreach (var asset in AssetDataBase.AllAssets)
            {
                var type = AssetDataBase.GetType(asset.Key);

                if (_currentFilter != null && type != _currentFilter)
                    continue;

                ImGui.PushStyleColor(ImGuiCol.Button, _assetColors[type]);
                ImGui.PushStyleColor(ImGuiCol.ButtonHovered, _assetColors[type] * 1.1f);
                ImGui.PushStyleColor(ImGuiCol.ButtonActive, _assetColors[type] * 0.9f);

                if (ImGui.Button(Path.GetFileNameWithoutExtension(asset.Value), new Vector2(_buttonSize, _buttonSize)))
                {
                    
                }

                ImGui.PopStyleColor();
                ImGui.PopStyleColor();
                ImGui.PopStyleColor();

                if (ImGui.IsItemHovered())
                {
                    ImGui.SetTooltip(asset.Value);
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

            ImGui.End();
        }
    }
}

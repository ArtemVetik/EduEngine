using ImGuiNET;

namespace EduEngine.Editor
{
    internal static class ImGuiEx
    {
        private static string _filter = string.Empty;

        public static void RenderAssetSelect(Type assetType, Asset? currentAsset, string fieldName, Action<Asset?> onSelected)
        {
            if (currentAsset != null && currentAsset.IsValid == false)
            {
                onSelected?.Invoke(null);
                return;
            }

            var assets = AssetDataBase.AllAssets.Where(asset => asset.Value.Asset != null &&
                                                                            asset.Value.Asset.GetType() == assetType);

            if (ImGui.BeginCombo(fieldName, currentAsset == null ? "null" : AssetDataBase.GetAssetData(currentAsset.GUID).LocalPath))
            {
                ImGui.InputText("Search", ref _filter, 256);
                ImGui.Separator();

                assets = FilterAssets(assetType);

                if (ImGui.Selectable("null"))
                {
                    onSelected?.Invoke(null);
                }

                foreach (var asset in assets)
                {
                    if (ImGui.Selectable(asset.Value.LocalPath))
                    {
                        onSelected?.Invoke(asset.Value.Asset);
                    }
                }
                ImGui.EndCombo();
            }
        }

        public static void RenderGameObjectSelect(GameObject? currentGameObject, string fieldName, Action<GameObject?> onSelected, List<GameObject> ignoreObjects = null)
        {
            if (currentGameObject != null && currentGameObject.IsAlive == false)
            {
                onSelected?.Invoke(null);
                return;
            }

            if (ImGui.BeginCombo(fieldName, currentGameObject == null ? "null" : currentGameObject.Name))
            {
                ImGui.InputText("Search", ref _filter, 256);
                ImGui.Separator();

                if (ImGui.Selectable("null"))
                {
                    onSelected?.Invoke(null);
                }

                int idx = 0;
                foreach (var option in FilterGameObjects(ignoreObjects))
                {
                    if (ImGui.Selectable($"{option.Name}##{idx++}"))
                    {
                        onSelected?.Invoke(option);
                    }
                }

                ImGui.EndCombo();
            }
            else
            {
                _filter = string.Empty;
            }
        }

        private static IEnumerable<KeyValuePair<string, AssetData>> FilterAssets(Type assetType)
        {
            foreach (var asset in AssetDataBase.AllAssets)
            {
                if (asset.Value.Asset == null)
                    continue;

                if (asset.Value.Asset.GetType() != assetType)
                    continue;

                if (string.IsNullOrEmpty(_filter) || asset.Value.LocalPath.Contains(_filter, StringComparison.OrdinalIgnoreCase))
                    yield return asset;
            }
        }

        private static IEnumerable<GameObject> FilterGameObjects(List<GameObject> ignoreObjects)
        {
            foreach (var go in SceneManager.CurrentScene.Objects)
            {
                if (ignoreObjects != null && ignoreObjects.Contains(go))
                    continue;

                if (string.IsNullOrEmpty(_filter) || go.Name.Contains(_filter, StringComparison.OrdinalIgnoreCase))
                    yield return go;
            }
        }
    }
}

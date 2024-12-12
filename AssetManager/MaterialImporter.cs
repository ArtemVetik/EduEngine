using Newtonsoft.Json;

namespace EduEngine
{
    public static class MaterialImporter
    {
        public static void CreateMaterialFile(string materialName, MaterialData data)
        {
            var fullPath = $"{AssetDataBase.AssetsPath}{materialName}.edumat";

            File.WriteAllText(fullPath, JsonConvert.SerializeObject(data, Formatting.Indented));
            AssetDataBase.Resolve();
        }

        public static Material LoadMaterial(string guid)
        {
            var assetData = AssetDataBase.GetAssetData(guid);
            var materialData = JsonConvert.DeserializeObject<MaterialData>(File.ReadAllText(assetData.GlobalPath));
            
            var material = new Material(guid);

            if (string.IsNullOrEmpty(materialData.TextureGUID) == false && AssetDataBase.HasGUID(materialData.TextureGUID))
            {
                var textureAsset = AssetDataBase.GetAssetData(materialData.TextureGUID);
                material.SetMainTexture(textureAsset.Asset as Texture);
            }
            
            return material;
        }

        public static void SaveMaterial(Material material)
        {
            if (material == null)
                throw new NullReferenceException();

            var materialPath = AssetDataBase.HasGUID(material.GUID)
                ? AssetDataBase.GetAssetData(material.GUID).GlobalPath
                : null;

            if (materialPath == null)
                return;

            var materialData = new MaterialData();

            if (material.MainTexture == null || material.MainTexture.IsValid == false)
                materialData.TextureGUID = null;
            else
                materialData.TextureGUID = material.MainTexture.GUID;

            File.WriteAllText(materialPath, JsonConvert.SerializeObject(materialData));
        }
    }

    public class MaterialData
    {
        public string TextureGUID { get; set; }
    }
}
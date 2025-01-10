using Newtonsoft.Json;
using System.Numerics;

namespace EduEngine
{
    public static class MaterialImporter
    {
        public static void CreateMaterialFile(string materialName, MaterialData data)
        {
            if (materialName.StartsWith("\\") == false)
                materialName = "\\" + materialName;

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

            material.DiffuseAlbedo = materialData.DiffuseAlbedo;
            material.FresnelR0 = materialData.FresnelR0;
            material.Roughness = materialData.Roughness;

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

            materialData.DiffuseAlbedo = material.DiffuseAlbedo;
            materialData.FresnelR0 = material.FresnelR0;
            materialData.Roughness = material.Roughness;

            File.WriteAllText(materialPath, JsonConvert.SerializeObject(materialData, Formatting.Indented));
        }
    }

    public class MaterialData
    {
        public string TextureGUID { get; set; }
        public Vector4 DiffuseAlbedo { get; set; }
        public Vector3 FresnelR0 { get; set; }
        public float Roughness { get; set; }
    }
}
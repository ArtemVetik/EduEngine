
namespace EduEngine
{
    public enum AssetType
    {
        Scene,
        Script,
        Mesh,
        Texture,
        Material,
        Invalid,
    }

    public static class AssetTypeExtentions
    {
        public static string ToFileExtention(this AssetType type)
        {
            switch (type)
            {
                case AssetType.Scene: return ".scene";
                case AssetType.Mesh: return ".fbx";
                case AssetType.Texture: return ".dds";
                case AssetType.Material: return ".edumat";
                case AssetType.Invalid: return string.Empty;
                default: throw new ArgumentOutOfRangeException(nameof(type));
            }
        }

        public static AssetType ToAssetType(this string extention)
        {
            switch (extention)
            {
                case ".scene": return AssetType.Scene;
                case ".fbx": return AssetType.Mesh;
                case ".cs": return AssetType.Script;
                case ".dds": return AssetType.Texture;
                case ".edumat": return AssetType.Material;
                default: return AssetType.Invalid;
            }
        }
    }
}

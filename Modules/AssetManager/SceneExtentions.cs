namespace EduEngine
{
    public static class SceneExtentions
    {
        public static void ReloadCurrentScene()
        {
            var guid = SceneManager.CurrentScene.GUID;
            AssetDataBase.GetAssetData(guid);
            SceneImporter.LoadScene(guid, true);
        }
    }
}
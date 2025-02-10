using ImGuiNET;

namespace EduEngine.Editor
{
    internal class RenderResourcesInfo : IDisposable
    {
        private RenderResourcesInfoWrapper _renderResourcesInfo = new();
        private bool _onlyUsed = false;

        public bool Show = false;

        public void Render()
        {
            if (Show)
            {
                ImGui.Begin("Render Resources", ref Show, ImGuiWindowFlags.HorizontalScrollbar);
                ImGui.Checkbox("Only Used", ref _onlyUsed);
                ImGui.TextUnformatted(_renderResourcesInfo.GetObjectsInfo(_onlyUsed));
                ImGui.End();
            }
        }

        public void Dispose()
        {
            _renderResourcesInfo.Dispose();
        }
    }
}

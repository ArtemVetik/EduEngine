using ImGuiNET;

namespace EduEngine.Scripts
{
    public class SampleUI : Component
    {
        public SampleUI(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            OverlayUI.Initialize();
        }

        public override void Update()
        {
            ImGui.ShowDemoWindow();
        }

        public override void OnDestroy()
        {
            OverlayUI.Destroy();
        }
    }
}

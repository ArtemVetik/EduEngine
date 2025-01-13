using EduEngine;
using ImGuiNET;
using System.Numerics;

namespace ParticlesDemo
{
    internal class UIRoot : Component
    {
        public UIRoot(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            OverlayUI.Initialize();
        }

        public override void Update()
        {
            ImGui.SetNextWindowPos(new Vector2(0.05f * Screen.Size.X, 0.1f * Screen.Size.Y), ImGuiCond.Always);
            ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);

            ImGui.Begin("##InvisibleWindow", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);

            ImGui.Text("----GPU PARTICLE SYSTEM DEMO----\n" +
                       "Move:   'W' 'A' 'S' 'D' + 'Q' 'E'\n" +
                       "Rotate: RMB + mouse move");

            ImGui.End();
        }

        public override void OnDestroy()
        {
            OverlayUI.Destroy();
        }
    }
}

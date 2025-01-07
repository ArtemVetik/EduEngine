using ImGuiNET;

namespace EduEngine.Editor
{
    internal class RenderSettingsWindow
    {
        internal bool Show = false;

        public void Render()
        {
            if (!Show)
                return;

            ImGui.Begin("RenderSettingsWindow", ref Show);

            var shadowDistance = RenderSettingsInterop.ShadowDistance;
            if (ImGui.InputFloat("Shadow Distance", ref shadowDistance))
                RenderSettingsInterop.ShadowDistance = shadowDistance;

            var asyncComputeParticles = RenderSettingsInterop.AsyncComputeParticles;
            if (ImGui.Checkbox("Async Compute Particles", ref asyncComputeParticles))
                RenderSettingsInterop.AsyncComputeParticles = asyncComputeParticles;

            ImGui.End();
        }
    }
}

using EduEngine;
using ImGuiNET;
using System.Numerics;

namespace LightingDemo
{
    public class InfoUI : Component
    {
        [SerializeField] private GameObject _pointlightSpawnerObject;
        [SerializeField] private GameObject _spotlightSpawnerObject;

        private PointLightSpawner _pointlightSpawner;
        private SpotlightSpawner _spotlightSpawner;

        public InfoUI(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            OverlayUI.Initialize();

            _pointlightSpawner = _pointlightSpawnerObject.GetComponent<PointLightSpawner>();
            _spotlightSpawner = _spotlightSpawnerObject.GetComponent<SpotlightSpawner>();
        }

        public override void Update()
        {
            var drawList = ImGui.GetBackgroundDrawList();

            Vector2 position = new Vector2(0.1f * Screen.Size.X, 0.1f * Screen.Size.Y);
            uint color = ImGui.GetColorU32(new Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            drawList.AddText(position, color, $"Directional light with shadows: 1\n" +
                $"Point lights: {_pointlightSpawner.LightCount}\n" +
                $"Spotlight:    {_spotlightSpawner.LightCount}\n" +
                $"(no instansing)");

        }

        public override void OnDestroy()
        {
            OverlayUI.Destroy();
        }
    }
}

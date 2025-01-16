using EduEngine;
using ImGuiNET;
using System.Numerics;

namespace SplitScreenDemo
{
    public class PlayerUI : Component
    {
        [SerializeField] private PlayerType _playerType;
        [SerializeField] private GameObject _playerObject;

        private PlayerMovement _movement;

        public PlayerUI(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _movement = _playerObject.GetComponent<PlayerMovement>();
        }

        public override void Update()
        {
            var posX = _playerType == PlayerType.Left ? 0.02f * Screen.Size.X : 0.52f * Screen.Size.X;
            var posY = 0.9f * Screen.Size.Y;

            ImGui.SetNextWindowPos(new Vector2(posX, 0.1f * Screen.Size.Y), ImGuiCond.Always);
            ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);

            ImGui.Begin($"##InvisibleInfoWindow{GUID}", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);

            if (_playerType == PlayerType.Left)
                ImGui.Text("Move: 'W' 'A' 'S' 'D'\n" +
                           "Jump: 'Space'");
            else
                ImGui.Text("Move: 'Left' 'Right' 'Up' 'Down' arrows\n" +
                           "Jump: 'Right Ctrl'");
            ImGui.End();

            ImGui.SetNextWindowPos(new Vector2(posX, posY), ImGuiCond.Always);
            ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);

            ImGui.Begin($"##InvisibleStaminaWindow{GUID}", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);

            ImGui.Text("Jump Stamina");
            DrawSlider(Math.Clamp(_movement.JumpStamina * 2, 0.0f, 1.0f));
            ImGui.SameLine();
            ImGui.Spacing();
            ImGui.SameLine();
            DrawSlider(Math.Clamp((_movement.JumpStamina - 0.5f) * 2, 0.0f, 1.0f));

            ImGui.End();

            if (_playerType == PlayerType.Left)
            {
                bool reload = false;
                ImGui.SetNextWindowPos(new Vector2(0.4f * Screen.Size.X, 0.9f * Screen.Size.Y), ImGuiCond.Always);
                ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);
                ImGui.Begin($"##InvisibleButtonWindow{GUID}", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);
                if (ImGui.Button("Restart"))
                    reload = true;
                ImGui.End();

                if (reload)
                    SceneExtentions.ReloadCurrentScene();
            }
        }

        private void DrawSlider(float fill)
        {
            var sliderSize = new Vector2(50, 20);
            var sliderPos = ImGui.GetCursorScreenPos();
            var drawList = ImGui.GetWindowDrawList();

            drawList.AddRectFilled(sliderPos, sliderPos + sliderSize, 0x66FFBCCE);

            var fillWidth = fill * sliderSize.X;
            var color = fill >= 1.0f ? 0xFF9FFF73 : 0xC2FEFFFF;
            drawList.AddRectFilled(sliderPos, sliderPos + new Vector2(fillWidth, sliderSize.Y), color);

            ImGui.InvisibleButton($"##{GUID}", sliderSize);

            var textPos = sliderPos + new Vector2(sliderSize.X / 2 - 20, sliderSize.Y / 2 - 8);
            drawList.AddText(textPos, 0xFFFFFFFF, $"{fill:F2}");
        }
    }
}

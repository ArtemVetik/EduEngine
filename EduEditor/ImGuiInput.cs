using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class ImGuiInput
    {
        private const float FirstInputDelay = 1.0f / 2.0f;
        private const float InputDelay = 1.0f / 30.0f;

        private struct PressedKey
        {
            public KeyCode keyCode;
            public float NextPressTime;
        }

        private PressedKey _pressedKey;

        public void ProcessInput()
        {
            ImGuiIOPtr io = ImGui.GetIO();
            io.DisplaySize = EditorRenderEngineInterop.GetEditorSize();
            io.DisplayFramebufferScale = Vector2.One;
            io.DeltaTime = EduTime.DeltaTime;
            io.AddMousePosEvent(Input.MousePosition.X, Input.MousePosition.Y);
            io.AddMouseButtonEvent(0, Input.MouseButtonPressed(MouseCode.Mouse0));
            io.AddMouseButtonEvent(1, Input.MouseButtonPressed(MouseCode.Mouse1));
            io.AddMouseButtonEvent(2, Input.MouseButtonPressed(MouseCode.Mouse2));
            io.AddMouseButtonEvent(3, Input.MouseButtonPressed(MouseCode.Mouse3));
            io.AddMouseButtonEvent(4, Input.MouseButtonPressed(MouseCode.Mouse4));
            io.AddMouseWheelEvent(0f, Input.ScrollDelta.Y);

            var keyCodes = Enum.GetValues<KeyCode>();

            for (int i = 0; i < keyCodes.Length; i++)
            {
                if (Input.KeyDown(keyCodes[i]))
                {
                    io.AddInputCharacter(keyCodes[i].ToChar());
                    io.AddKeyEvent(keyCodes[i].ToImGuiKey(), true);

                    _pressedKey.keyCode = keyCodes[i];
                    _pressedKey.NextPressTime = EduTime.TotalTime + FirstInputDelay;
                }

                if (Input.KeyPressed(keyCodes[i]))
                {
                    if (keyCodes[i] == _pressedKey.keyCode && EduTime.TotalTime > _pressedKey.NextPressTime)
                    {
                        io.AddInputCharacter(keyCodes[i].ToChar());
                        io.AddKeyEvent(keyCodes[i].ToImGuiKey(), true);

                        _pressedKey.NextPressTime = EduTime.TotalTime + InputDelay;
                    }
                }

                if (Input.KeyUp(keyCodes[i]))
                {
                    io.AddKeyEvent(keyCodes[i].ToImGuiKey(), false);

                    if (keyCodes[i] == _pressedKey.keyCode)
                        _pressedKey = default;
                    else
                        _pressedKey.NextPressTime = EduTime.TotalTime + FirstInputDelay;
                }
            }
        }
    }
}

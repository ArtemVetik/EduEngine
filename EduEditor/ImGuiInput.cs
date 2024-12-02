using ImGuiNET;
using System.Numerics;

namespace EduEngine.Editor
{
    internal class ImGuiInput
    {
        private const float FirstInputDelay = 0.5f;
        private const float InputDelay = 0.05f;

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
            io.DeltaTime = EditorTime.DeltaTime;
            io.KeyRepeatRate = InputDelay;
            io.KeyRepeatDelay = FirstInputDelay;
            io.AddMousePosEvent(Input.Editor.MousePosition.X, Input.Editor.MousePosition.Y);
            io.AddMouseButtonEvent(0, Input.Editor.MouseButtonPressed(MouseCode.Mouse0));
            io.AddMouseButtonEvent(1, Input.Editor.MouseButtonPressed(MouseCode.Mouse1));
            io.AddMouseButtonEvent(2, Input.Editor.MouseButtonPressed(MouseCode.Mouse2));
            io.AddMouseButtonEvent(3, Input.Editor.MouseButtonPressed(MouseCode.Mouse3));
            io.AddMouseButtonEvent(4, Input.Editor.MouseButtonPressed(MouseCode.Mouse4));
            io.AddMouseWheelEvent(0f, Input.Editor.ScrollDelta.Y);

            var keyCodes = Enum.GetValues<KeyCode>();

            for (int i = 0; i < keyCodes.Length; i++)
            {
                if (Input.Editor.KeyDown(keyCodes[i]))
                {
                    if (keyCodes[i] == KeyCode.LSHIFT || keyCodes[i] == KeyCode.RSHIFT)
                        io.KeyShift = true;

                    io.AddInputCharacter(keyCodes[i].ToChar(io.KeyShift ^ Input.Editor.CapsLockEnabled()));
                    io.AddKeyEvent(keyCodes[i].ToImGuiKey(), true);

                    _pressedKey.keyCode = keyCodes[i];
                    _pressedKey.NextPressTime = EditorTime.TotalTime + FirstInputDelay;
                }
                if (Input.Editor.KeyPressed(keyCodes[i]))
                {
                    if (keyCodes[i] == _pressedKey.keyCode && EditorTime.TotalTime >= _pressedKey.NextPressTime)
                    {
                        io.AddInputCharacter(keyCodes[i].ToChar(io.KeyShift ^ Input.Editor.CapsLockEnabled()));
                        io.AddKeyEvent(keyCodes[i].ToImGuiKey(), true);

                        _pressedKey.NextPressTime = EditorTime.TotalTime + InputDelay;
                    }
                }

                if (Input.Editor.KeyUp(keyCodes[i]))
                {
                    io.AddKeyEvent(keyCodes[i].ToImGuiKey(), false);

                    if (keyCodes[i] == KeyCode.LSHIFT || keyCodes[i] == KeyCode.RSHIFT)
                        io.KeyShift = false;

                    if (keyCodes[i] == _pressedKey.keyCode)
                        _pressedKey = default;
                    else
                        _pressedKey.NextPressTime = EditorTime.TotalTime + FirstInputDelay;
                }
            }
        }
    }
}

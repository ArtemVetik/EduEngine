using EduEngine;
using ImGuiNET;
using System.Numerics;

namespace EduEngine
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

        public void ProcessInput(Input.InputInstance input, Vector2 screenSize, float deltaTime, float totalTime)
        {
            ImGuiIOPtr io = ImGui.GetIO();
            io.DisplaySize = screenSize;
            io.DisplayFramebufferScale = Vector2.One;
            io.DeltaTime = deltaTime;
            io.KeyRepeatRate = InputDelay;
            io.KeyRepeatDelay = FirstInputDelay;
            io.AddMousePosEvent(input.MousePosition.X, input.MousePosition.Y);
            io.AddMouseButtonEvent(0, input.MouseButtonPressed(MouseCode.Mouse0));
            io.AddMouseButtonEvent(1, input.MouseButtonPressed(MouseCode.Mouse1));
            io.AddMouseButtonEvent(2, input.MouseButtonPressed(MouseCode.Mouse2));
            io.AddMouseButtonEvent(3, input.MouseButtonPressed(MouseCode.Mouse3));
            io.AddMouseButtonEvent(4, input.MouseButtonPressed(MouseCode.Mouse4));
            io.AddMouseWheelEvent(0f, input.ScrollDelta.Y);

            var keyCodes = Enum.GetValues<KeyCode>();

            for (int i = 0; i < keyCodes.Length; i++)
            {
                if (input.KeyDown(keyCodes[i]))
                {
                    if (keyCodes[i] == KeyCode.LSHIFT || keyCodes[i] == KeyCode.RSHIFT)
                        io.KeyShift = true;

                    io.AddInputCharacter(keyCodes[i].ToChar(io.KeyShift ^ input.CapsLockEnabled()));
                    io.AddKeyEvent(keyCodes[i].ToImGuiKey(), true);

                    _pressedKey.keyCode = keyCodes[i];
                    _pressedKey.NextPressTime = totalTime + FirstInputDelay;
                }
                if (input.KeyPressed(keyCodes[i]))
                {
                    if (keyCodes[i] == _pressedKey.keyCode && totalTime >= _pressedKey.NextPressTime)
                    {
                        io.AddInputCharacter(keyCodes[i].ToChar(io.KeyShift ^ input.CapsLockEnabled()));
                        io.AddKeyEvent(keyCodes[i].ToImGuiKey(), true);

                        _pressedKey.NextPressTime = totalTime + InputDelay;
                    }
                }

                if (input.KeyUp(keyCodes[i]))
                {
                    io.AddKeyEvent(keyCodes[i].ToImGuiKey(), false);

                    if (keyCodes[i] == KeyCode.LSHIFT || keyCodes[i] == KeyCode.RSHIFT)
                        io.KeyShift = false;

                    if (keyCodes[i] == _pressedKey.keyCode)
                        _pressedKey = default;
                    else
                        _pressedKey.NextPressTime = totalTime + FirstInputDelay;
                }
            }
        }
    }
}

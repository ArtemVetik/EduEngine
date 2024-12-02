using System.Numerics;

namespace EduEngine
{
    public static class Input
    {
        public static InputInstance Runtime = new InputInstance(false);
        public static InputInstance Editor = new InputInstance(true);

        public class InputInstance
        {
            private readonly InputSystemInterop _input;

            internal InputInstance(bool editor)
            {
                _input = new InputSystemInterop(editor);
            }

            public bool AnyKeyPressed => _input.IsAnyKeyPressed();
            public bool KeyPressed(KeyCode key) => _input.IsKeyPressed((byte)key);
            public bool KeyDown(KeyCode key) => _input.IsKeyDown((byte)key);
            public bool KeyUp(KeyCode key) => _input.IsKeyUp((byte)key);
            public bool CapsLockEnabled() => _input.IsCapsLockEnabled();
            public Vector2 MousePosition => _input.GetCursorPos();
            public Vector2 MouseDelta
            {
                get
                {
                    var mouseState = _input.GetMouseState();
                    return new Vector2(mouseState.lX, mouseState.lY);
                }
            }
            public Vector2 ScrollDelta
            {
                get
                {
                    var mouseState = _input.GetMouseState();
                    return new Vector2(0, mouseState.lZ);
                }
            }
            public bool MouseButtonPressed(MouseCode mouseCode)
            {
                var mouseState = _input.GetMouseState();
                return (mouseState.rgbButtons[(byte)mouseCode] & 0x80) != 0;
            }
            public bool MouseButtonDown(MouseCode mouseCode)
            {
                var prevMouseState = _input.GetPrevMouseState();
                var mouseState = _input.GetMouseState();

                return (prevMouseState.rgbButtons[(byte)mouseCode] & 0x80) == 0 &&
                       (mouseState.rgbButtons[(byte)mouseCode] & 0x80) != 0;
            }
            public bool MouseButtonUp(MouseCode mouseCode)
            {
                var prevMouseState = _input.GetPrevMouseState();
                var mouseState = _input.GetMouseState();

                return (prevMouseState.rgbButtons[(byte)mouseCode] & 0x80) != 0 &&
                       (mouseState.rgbButtons[(byte)mouseCode] & 0x80) == 0;
            }
        }
    }
}

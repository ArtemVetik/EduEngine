using System.Numerics;

namespace EduEngine
{
    public static class Input
    {
        public static bool AnyKeyPressed => InputSystemInterop.IsAnyKeyPressed();
        public static bool KeyPressed(KeyCode key) => InputSystemInterop.IsKeyPressed((byte)key);
        public static Vector2 MousePosition => InputSystemInterop.GetCursorPos();
        public static Vector2 MouseDelta
        {
            get
            {
                var mouseState = InputSystemInterop.GetMouseState();
                return new Vector2(mouseState.lX, mouseState.lY);
            }
        }
        public static Vector2 ScrollDelta
        {
            get
            {
                var mouseState = InputSystemInterop.GetMouseState();
                return new Vector2(0, mouseState.lZ);
            }
        }
        public static bool MouseButtonPressed(MouseCode mouseCode)
        {
            var mouseState = InputSystemInterop.GetMouseState();
            return (mouseState.rgbButtons[(byte)mouseCode] & 0x80) != 0;
        }
        public static bool MouseButtonDown(MouseCode mouseCode)
        {
            var prevMouseState = InputSystemInterop.GetPrevMouseState();
            var mouseState = InputSystemInterop.GetMouseState();

            return (prevMouseState.rgbButtons[(byte)mouseCode] & 0x80) == 0 && 
                   (mouseState.rgbButtons[(byte)mouseCode] & 0x80) != 0;
        }
        public static bool MouseButtonUp(MouseCode mouseCode)
        {
            var prevMouseState = InputSystemInterop.GetPrevMouseState();
            var mouseState = InputSystemInterop.GetMouseState();

            return (prevMouseState.rgbButtons[(byte)mouseCode] & 0x80) != 0 &&
                   (mouseState.rgbButtons[(byte)mouseCode] & 0x80) == 0;
        }
    }
}

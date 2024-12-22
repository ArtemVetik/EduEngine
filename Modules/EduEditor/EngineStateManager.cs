namespace EduEngine.Editor
{
    public static class EngineStateManager
    {
        public static EngineState CurrentState { get; internal set; } = EngineState.Editor;
        public static bool InspectScene { get; internal set; } = false;
    }
}

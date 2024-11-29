using ImGuizmoNET;

namespace EduEngine.Editor
{
    internal class GuizmoRenderer
    {
        private OPERATION _operation = OPERATION.TRANSLATE;

        public void SetOperation(OPERATION operation)
        {
            _operation = operation;
        }

        public void Render(GameObject selected, EditorCamera camera, float x, float y, float w, float h)
        {
            if (selected == null)
                return;

            var view = camera.ViewMatrix;
            var proj = camera.ProjectionMatrix;
            var world = selected.Transform.WorldMatrix;

            ImGuizmo.SetRect(x, y, w, h);

            if (ImGuizmo.Manipulate(ref view.M11, ref proj.M11, _operation, MODE.LOCAL, ref world.M11))
            {
                selected.Transform.SetWorldMatrix(world);
            }
        }
    }
}

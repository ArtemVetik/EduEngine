using ImGuizmoNET;
using System.Numerics;

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
            var parentWorld = selected.Parent == null ? Matrix4x4.Identity : selected.Parent.Transform.WorldMatrix;
            var world = selected.Transform.WorldMatrix;

            ImGuizmo.SetRect(x, y, w, h);

            if (ImGuizmo.Manipulate(ref view.M11, ref proj.M11, _operation, MODE.LOCAL, ref world.M11))
            {
                Matrix4x4.Invert(parentWorld, out Matrix4x4 parentInv);
                selected.Transform.SetLocalWorldMatrix(world * parentInv);
            }
        }
    }
}

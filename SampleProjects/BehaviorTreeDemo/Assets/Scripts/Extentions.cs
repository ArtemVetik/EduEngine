using System.Numerics;

namespace BehaviorTreeDemo
{
    public class Extentions
    {
        public static Quaternion LookRotation(Vector3 forward, Vector3 up)
        {
            forward = Vector3.Normalize(forward);
            Vector3 right = Vector3.Normalize(Vector3.Cross(up, forward));
            up = Vector3.Cross(forward, right);

            Matrix4x4 rotationMatrix = new Matrix4x4(
                right.X, right.Y, right.Z, 0,
                up.X, up.Y, up.Z, 0,
                forward.X, forward.Y, forward.Z, 0,
                0, 0, 0, 1
            );

            return Quaternion.CreateFromRotationMatrix(rotationMatrix);
        }
    }
}

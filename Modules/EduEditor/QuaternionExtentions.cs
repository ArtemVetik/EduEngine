using System.Numerics;

namespace EduEngine.Editor
{
    internal static class QuaternionExtentions
    {
        public static Vector3 ToEuler(this Quaternion q)
        {
            q = Quaternion.Normalize(q);
            float sinr_cosp = 2 * (q.W * q.X + q.Y * q.Z);
            float cosr_cosp = 1 - 2 * (q.X * q.X + q.Y * q.Y);
            float roll = MathF.Atan2(sinr_cosp, cosr_cosp);

            float sinp = 2 * (q.W * q.Y - q.Z * q.X);
            float pitch = MathF.Abs(sinp) >= 1
                ? MathF.CopySign(MathF.PI / 2, sinp)
                : MathF.Asin(sinp);

            float siny_cosp = 2 * (q.W * q.Z + q.X * q.Y);
            float cosy_cosp = 1 - 2 * (q.Y * q.Y + q.Z * q.Z);
            float yaw = MathF.Atan2(siny_cosp, cosy_cosp);

            return new Vector3(
                yaw * (180.0f / MathF.PI),
                pitch * (180.0f / MathF.PI),
                roll * (180.0f / MathF.PI)
            );
        }

        public static Quaternion ToEulerQuaternion(this Vector3 euler)
        {
            float yaw = euler.X * (MathF.PI / 180.0f);
            float pitch = euler.Y * (MathF.PI / 180.0f);
            float roll = euler.Z * (MathF.PI / 180.0f);

            float cy = MathF.Cos(yaw * 0.5f);
            float sy = MathF.Sin(yaw * 0.5f);
            float cp = MathF.Cos(pitch * 0.5f);
            float sp = MathF.Sin(pitch * 0.5f);
            float cr = MathF.Cos(roll * 0.5f);
            float sr = MathF.Sin(roll * 0.5f);

            return new Quaternion
            (
                sr * cp * cy - cr * sp * sy,
                cr * sp * cy + sr * cp * sy,
                cr * cp * sy - sr * sp * cy,
                cr * cp * cy + sr * sp * sy
            );
        }
    }
}

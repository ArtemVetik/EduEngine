using EduEngine;
using System.Numerics;

namespace LightingDemo
{
    public class YRotate : Component
    {
        [SerializeField] private float _speed = 5.0f;

        public YRotate(GameObject parent)
            : base(parent)
        { }

        public override void Update()
        {
            GameObject.Transform.Rotation *= Quaternion.CreateFromAxisAngle(Vector3.UnitY, _speed * EduTime.DeltaTime);
        }
    }
}

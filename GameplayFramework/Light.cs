using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public class Light : Component, IDisposable
    {
        [SerializeField] private LightType _type;
        [SerializeField, Color] private Vector3 _color;
        [SerializeField] private float _power;
        [SerializeField] private float _falloffStart;
        [SerializeField] private float _falloffEnd;
        [SerializeField] private float _spotPower;

        private LightWrapper _lightWrapper;

        public Light(GameObject parent)
            : base(parent)
        {
            _lightWrapper = new LightWrapper();
        }

        public void Dispose()
        {
            _lightWrapper.Dispose();
        }

        public override void Update()
        {
            _lightWrapper.SetPosition(GameObject.Transform.Position);
            _lightWrapper.SetDirection(GameObject.Transform.Forward);
        }

        public override void UpdateEditor()
        {
            Update();
            _lightWrapper.DebugDraw();
        }

        public override void OnAddComponent()
        {
            _lightWrapper.SetLightType(_type);
            _lightWrapper.SetStrength(_color * _power);
            _lightWrapper.SetFalloffStart(_falloffStart);
            _lightWrapper.SetFalloffEnd(_falloffEnd);
            _lightWrapper.SetSpotPower(_spotPower);
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            OnAddComponent();
        }
    }
}

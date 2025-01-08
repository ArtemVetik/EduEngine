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

        public LightType Type
        {
            get => _type;
            set => _lightWrapper.SetLightType(_type = value);
        }

        public Vector3 Color
        {
            get => _color;
            set => _lightWrapper.SetStrength((_color = value) * _power);
        }

        public float Power
        {
            get => _power;
            set => _lightWrapper.SetStrength(_color * (_power = value));
        }

        public float FalloffStart
        {
            get => _falloffStart;
            set => _lightWrapper.SetFalloffStart(_falloffStart = value);
        }

        public float FalloffEnd
        {
            get => _falloffStart;
            set => _lightWrapper.SetFalloffEnd(_falloffEnd = value);
        }

        public float SpotPower
        {
            get => _spotPower;
            set => _lightWrapper.SetSpotPower(_spotPower = value);
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

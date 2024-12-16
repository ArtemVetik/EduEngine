using System.Numerics;

namespace EduEngine
{
    public class Light : Component, IDisposable, ISerializeCallback
    {
        [SerializeField] private LightType _type;
        [SerializeField] private float _r;
        [SerializeField] private float _g;
        [SerializeField] private float _b;
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
            _lightWrapper.SetStrength(new Vector3(_r, _g, _b));
            _lightWrapper.SetFalloffStart(_falloffStart);
            _lightWrapper.SetFalloffEnd(_falloffEnd);
            _lightWrapper.SetSpotPower(_spotPower);
        }

        void ISerializeCallback.OnDeserialize()
        {
            OnAddComponent();
        }
    }
}

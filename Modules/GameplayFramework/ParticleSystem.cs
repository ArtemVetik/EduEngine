using System.Diagnostics.CodeAnalysis;
using System.Numerics;

namespace EduEngine
{
    public class ParticleSystem : Component, IDisposable
    {
        [Serializable]
        public enum ShapeType
        {
            Rect = 0,
            Sphere = 1,
            SphereContour = 2,
        }

        [SerializeField] private bool _emissionEnabled;
        [SerializeField] private int _maxParticlesCount;
        [SerializeField] private ShapeType _shapeType;
        [SerializeField] private Vector3 _shapeSize;
        [SerializeField] private Vector3 _minStartSize;
        [SerializeField] private Vector3 _maxStartSize;
        [SerializeField] private Vector3 _endSize;
        [SerializeField] private Vector3 _offset;
        [SerializeField] private float _emissionRate;
        [SerializeField] private float _lifeTime;
        [SerializeField] private Vector3 _minStartVelocity;
        [SerializeField] private Vector3 _maxStartVelocity;
        [SerializeField] private bool _randVelocityOnBound;
        [SerializeField] private Vector3 _acceleration;
        [SerializeField, Color] private Vector4 _minStartColor;
        [SerializeField, Color] private Vector4 _maxStartColor;
        [SerializeField, Color] private Vector4 _endColor;
        [SerializeField] private float _dragTime;
        [SerializeField] private float _dragForce;
        [SerializeField] private Texture _colorTexture;
        [SerializeField] private bool _screenSpaceCollision;
        [SerializeField, Range(0.0f, 1.0f)] private float _collisionEnergyLoss;

        private NativeParticleSystemWrapper _nativeParticles;

        public ParticleSystem(GameObject parent)
            : base(parent)
        {
            _nativeParticles = new NativeParticleSystemWrapper();
        }

        public override void OnAddComponent()
        {
            InitParticles();
        }

        public override void Update()
        {
            _nativeParticles.SetCenterPosition(GameObject.Transform.Position + _offset);
        }

        public override void UpdateEditor()
        {
            _nativeParticles.SetCenterPosition(GameObject.Transform.Position + _offset);
        }

        public void Dispose()
        {
            _nativeParticles?.Dispose();
            _nativeParticles = null;
        }

        public void SetEmissionState(bool enabled)
        {
            _emissionEnabled = enabled;
            InitParticles();
        }

        public void SetStaticParticleData(ParticleData particleData)
        {
            _emissionRate = particleData.EmissionRate;
            _screenSpaceCollision = particleData.ScreenSpaceCollision;
            _emissionEnabled = particleData.EmissionEnabled;

            _shapeType = (ShapeType)particleData.ShapeType;
            _shapeSize = particleData.ShapeSize;
            _maxParticlesCount = (int)particleData.MaxParticles;
            _minStartSize = particleData.MinStartSize;
            _lifeTime = particleData.LifeTime;
            _maxStartSize = particleData.MaxStartSize;
            _dragForce = particleData.DragForce;
            _endSize = particleData.EndSize;
            _minStartColor = particleData.MinStartColor;
            _maxStartColor = particleData.MaxStartColor;
            _endColor = particleData.EndColor;
            _minStartVelocity = particleData.MinStartVelocity;
            _randVelocityOnBound = particleData.RandVelocityOnBound;
            _maxStartVelocity = particleData.MaxStartVelocity;
            _dragTime = particleData.DragTime;
            _acceleration = particleData.Acceleration;
            _collisionEnergyLoss = particleData.CollisionEnergyLoss;

            InitParticles();
        }

        public void SetColorTexture(Texture texture)
        {
            _colorTexture = texture;
            _nativeParticles.SetColorTexture(texture?.GetWrapper());
        }

        [DynamicDependency(nameof(OnFieldChangedByReflection))]
        private void OnFieldChangedByReflection(string fieldName)
        {
            InitParticles();
        }

        private void InitParticles()
        {
            ParticleData particleData = new ParticleData();
            particleData.EmissionRate = _emissionRate;
            particleData.ScreenSpaceCollision = _screenSpaceCollision;
            particleData.EmissionEnabled = _emissionEnabled;

            particleData.ShapeType = (uint)_shapeType;
            particleData.ShapeSize = _shapeSize;
            particleData.MaxParticles = (uint)_maxParticlesCount;
            particleData.MinStartSize = _minStartSize;
            particleData.LifeTime = _lifeTime;
            particleData.MaxStartSize = _maxStartSize;
            particleData.DragForce = _dragForce;
            particleData.EndSize = _endSize;
            particleData.MinStartColor = RGBtoHSV(_minStartColor);
            particleData.MaxStartColor = RGBtoHSV(_maxStartColor);
            particleData.EndColor = RGBtoHSV(_endColor);
            particleData.MinStartVelocity = _minStartVelocity;
            particleData.RandVelocityOnBound = _randVelocityOnBound;
            particleData.MaxStartVelocity = _maxStartVelocity;
            particleData.DragTime = _dragTime;
            particleData.Acceleration = _acceleration;
            particleData.CollisionEnergyLoss = _collisionEnergyLoss;

            _nativeParticles.SetParticlesData(ref particleData);
            _nativeParticles.SetColorTexture(_colorTexture?.GetWrapper());
        }

        private Vector3 RGBtoHCV(Vector3 RGB)
        {
            const float Epsilon = 1e-10f;
            Vector4 P = (RGB.Y < RGB.Z) 
                ? new Vector4(RGB.Z, RGB.Y, -1.0f, 2.0f / 3.0f) 
                : new Vector4(RGB.Y, RGB.Z, 0.0f, -1.0f / 3.0f);
            Vector4 Q = (RGB.X < P.X) 
                ? new Vector4(P.X, P.Y, P.W, RGB.X) 
                : new Vector4(RGB.X, P.Y, P.Z, P.X);

            float C = Q.X - MathF.Min(Q.W, Q.Y);
            float H = MathF.Abs((Q.W - Q.Y) / (6 * C + Epsilon) + Q.Z);
            return new Vector3(H, C, Q.X);
        }

        public Vector4 RGBtoHSV(Vector4 RGB)
        {
            const float Epsilon = 1e-10f;
            Vector3 HCV = RGBtoHCV(new Vector3(RGB.X, RGB.Y, RGB.Z));
            float S = HCV.Y / (HCV.Z + Epsilon);
            return new Vector4(HCV.X, S, HCV.Z, RGB.W);
        }
    }
}

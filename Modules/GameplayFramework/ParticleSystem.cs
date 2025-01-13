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
        [SerializeField] private ParticleData _test;

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

            particleData.ShapeType = (uint)_shapeType;
            particleData.ShapeSize = _shapeSize;
            particleData.MaxParticles = (uint)_maxParticlesCount;
            particleData.MinStartSize = _minStartSize;
            particleData.LifeTime = _lifeTime;
            particleData.MaxStartSize = _maxStartSize;
            particleData.DragForce = _dragForce;
            particleData.EndSize = _endSize;
            particleData.MinStartColor = _minStartColor;
            particleData.MaxStartColor = _maxStartColor;
            particleData.EndColor = _endColor;
            particleData.MinStartVelocity = _minStartVelocity;
            particleData.RandVelocityOnBound = _randVelocityOnBound;
            particleData.MaxStartVelocity = _maxStartVelocity;
            particleData.DragTime = _dragTime;
            particleData.Acceleration = _acceleration;

            _nativeParticles.SetParticlesData(ref particleData);
            _nativeParticles.SetColorTexture(_colorTexture?.GetWrapper());
        }
    }
}

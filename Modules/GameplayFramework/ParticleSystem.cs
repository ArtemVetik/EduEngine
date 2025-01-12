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
        [SerializeField] private Vector3 _offset;
        [SerializeField] private float _emissionRate;
        [SerializeField] private float _lifeTime;
        [SerializeField] private Vector3 _velocity;
        [SerializeField] private Vector3 _acceleration;
        [SerializeField, Color] private Vector4 _startColor;
        [SerializeField, Color] private Vector4 _endColor;
        [SerializeField] private Texture _colorTexture;
        [SerializeField] private bool _screenSpaceCollision;

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
            particleData.MaxParticles = (uint)_maxParticlesCount;
            particleData.ShapeType = (uint)_shapeType;
            particleData.LifeTime = _lifeTime;
            particleData.EmissionRate = _emissionRate;
            particleData.ShapeSize = _shapeSize;
            particleData.StartColor = _startColor;
            particleData.EndColor = _endColor;
            particleData.Velocity = _velocity;
            particleData.Acceleration = _acceleration;
            particleData.ScreenSpaceCollision = _screenSpaceCollision;

            _nativeParticles.SetParticlesData(ref particleData);
            _nativeParticles.SetColorTexture(_colorTexture?.GetWrapper());
        }
    }
}

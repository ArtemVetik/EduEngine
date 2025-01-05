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

        private NativeParticleSystemWrapper _nativeParticles;

        public ParticleSystem(GameObject parent)
            : base(parent)
        {
            _nativeParticles = new NativeParticleSystemWrapper();
        }

        public override void OnAddComponent()
        {
            InitParticles(true);
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
            InitParticles(fieldName == nameof(_maxParticlesCount));
        }

        private void InitParticles(bool setMaxValue)
        {
            if (setMaxValue)
                _nativeParticles.SetMaxParticles((uint)_maxParticlesCount);

            _nativeParticles.SetEmissionRate(_emissionRate);
            _nativeParticles.SetShapeType((uint)_shapeType);
            _nativeParticles.SetShapeSize(_shapeSize);
            _nativeParticles.SetLifeTime(_lifeTime);
            _nativeParticles.SetVelocity(_velocity);
            _nativeParticles.SetAcceleration(_acceleration);
            _nativeParticles.SetStartColor(_startColor);
            _nativeParticles.SetEndColor(_endColor);
        }
    }
}

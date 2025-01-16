using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    public class FallStairs : Component, ITriggerTarget
    {
        [SerializeField] private float _speed = 5.0f;

        private ParticleSystem _effect;
        private RigidBody _body;
        private Quaternion _targetRotation;
        private float _effectShowDelay;

        public FallStairs(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
            _effect = GameObject.GetComponentsInChildren<ParticleSystem>().First();
            _targetRotation = GameObject.Transform.Rotation;
        }

        public override void Update()
        {
            GameObject.Transform.Rotation = Quaternion.Lerp(GameObject.Transform.Rotation, _targetRotation, _speed * EduTime.DeltaTime);
            _body.SetTransform(GameObject.Transform.Position, GameObject.Transform.Rotation);

            if (_effectShowDelay > 0)
            {
                _effectShowDelay -= EduTime.DeltaTime;

                if (_effectShowDelay <= 0)
                {
                    _effect?.SetEmissionState(false);
                    _effect = null;
                }
            }
        }

        public void StartAction()
        {
            _targetRotation = Quaternion.Identity;
            _effect?.SetEmissionState(true);
            _effectShowDelay = 0.5f;
        }

        public void EndAction() { }
    }
}

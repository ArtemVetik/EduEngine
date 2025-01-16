using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    public class TriggerButton : Component, ITriggerTarget
    {
        [SerializeField] private Material _pressedMaterial;

        private Vector3 _defaultPosition;
        private Vector3 _targetPosition;
        private Renderer _renderer;
        private Material _defaultMaterial;
        private RigidBody _body;

        public TriggerButton(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _renderer = GameObject.GetComponent<Renderer>();
            _defaultMaterial = _renderer.Material;
            _body = GameObject.GetComponent<RigidBody>();
            _defaultPosition = GameObject.Transform.Position;
            _targetPosition = _defaultPosition;
        }

        public override void Update()
        {
            GameObject.Transform.Position = Vector3.Lerp(GameObject.Transform.Position, _targetPosition, 2.0f * EduTime.DeltaTime);
            _body.SetTransform(GameObject.Transform.Position, GameObject.Transform.Rotation);
        }

        public void StartAction()
        {
            _renderer.SetMaterial(_pressedMaterial);
            _targetPosition = _defaultPosition - Vector3.UnitY * 0.25f;
        }

        public void EndAction()
        {
            _renderer.SetMaterial(_defaultMaterial);
            _targetPosition = _defaultPosition;
        }
    }
}

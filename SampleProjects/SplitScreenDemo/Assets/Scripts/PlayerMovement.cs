using EduEngine;
using System.Numerics;

namespace SplitScreenDemo
{
    enum PlayerType
    {
        Left, Right
    }

    public class PlayerMovement : Component
    {
        private const float JumpRestoreTime = 8.0f;

        [SerializeField] private PlayerType _playerType;
        [SerializeField] private float _moveSpeed = 5.0f;
        [SerializeField] private float _rotationSpeed = 1.0f;
        [SerializeField] private float _jumpForce = 20.0f;

        private Jetpack _jetpack;
        private RigidBody _body;
        private bool _jetFly;
        private bool _jump;
        private float _currentMove;
        private float _currentRotate;
        private float _jumpStamina;
        private ParticleSystem _jumpEffect;

        public PlayerMovement(GameObject parent)
            : base(parent)
        { }

        public float JumpStamina => _jumpStamina;
        public bool HasJetpack => _jetpack != null;

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
            _jumpEffect = GameObject.GetComponentsInChildren<ParticleSystem>().First();
            _jumpStamina = 0.0f;
        }

        public override void Update()
        {
            _jumpStamina += EduTime.DeltaTime / JumpRestoreTime;
            if (_jumpStamina > 1.0f)
                _jumpStamina = 1.0f;

            _jumpEffect.SetEmissionState(false);
            if (_jump == false && _jumpStamina >= 0.5f && HasJetpack == false)
            {
                _jump = _playerType == PlayerType.Left ? Input.Runtime.KeyDown(KeyCode.SPACE) : Input.Runtime.KeyDown(KeyCode.RCONTROL);

                if (_jump)
                {
                    _jumpStamina -= 0.5f;
                    _jumpEffect.SetEmissionState(true);
                }
            }

            if (HasJetpack)
            {
                _jetFly = _playerType == PlayerType.Left ? Input.Runtime.KeyPressed(KeyCode.SPACE) : Input.Runtime.KeyPressed(KeyCode.RCONTROL);
                _jetpack.SetEffectActive(_jetFly);
            }
            else
                _jetFly = false;

            bool moveForward = _playerType == PlayerType.Left ? Input.Runtime.KeyPressed(KeyCode.W) : Input.Runtime.KeyPressed(KeyCode.UP);
            bool moveBackward = _playerType == PlayerType.Left ? Input.Runtime.KeyPressed(KeyCode.S) : Input.Runtime.KeyPressed(KeyCode.DOWN);
            bool rotateLeft = _playerType == PlayerType.Left ? Input.Runtime.KeyPressed(KeyCode.A) : Input.Runtime.KeyPressed(KeyCode.LEFT);
            bool rotateRight = _playerType == PlayerType.Left ? Input.Runtime.KeyPressed(KeyCode.D) : Input.Runtime.KeyPressed(KeyCode.RIGHT);

            _currentMove = moveForward ? _moveSpeed : moveBackward ? -_moveSpeed : 0;
            _currentRotate = rotateRight ? _rotationSpeed : rotateLeft ? -_rotationSpeed : 0;
        }

        public override void PhysicsUpdate()
        {
            if (_jump)
            {
                _body.Velocity = new Vector3(_body.Velocity.X, 0, _body.Velocity.Z);
                _body.AddForce(Vector3.UnitY * _jumpForce, ForceMode.IMPULSE);
                _jump = false;
            }
            if (_jetFly)
            {
                _body.AddForce(Vector3.UnitY, ForceMode.IMPULSE);
            }

            _body.Velocity = new Vector3(GameObject.Transform.Forward.X * _currentMove, _body.Velocity.Y, GameObject.Transform.Forward.Z * _currentMove);
            _body.AngularVelocity = new Vector3(0, _rotationSpeed * _currentRotate, 0);

            _body.AddForce(-Vector3.UnitY * 3, ForceMode.ACCELERATION);
        }

        public void AddJetpack(Jetpack jetpack)
        {
            _jetpack = jetpack;
        }
    }
}

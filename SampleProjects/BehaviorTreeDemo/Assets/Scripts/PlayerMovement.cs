using EduEngine;
using ImGuiNET;
using System.Numerics;

namespace BehaviorTreeDemo
{
    public class PlayerMovement : Character
    {
        [SerializeField] private GameObject _camera;
        [SerializeField] private float _moveSpeed = 5.0f;

        private Renderer _renderer;
        private RigidBody _body;

        public PlayerMovement(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
            _renderer = GameObject.GetComponentsInChildren<Renderer>().First();
        }

        public override void Update()
        {
            var moveDirection = Vector3.Zero;

            if (Input.Runtime.KeyPressed(KeyCode.W))
                moveDirection.Z = 1;
            if (Input.Runtime.KeyPressed(KeyCode.S))
                moveDirection.Z = -1;
            if (Input.Runtime.KeyPressed(KeyCode.A))
                moveDirection.X = -1;
            if (Input.Runtime.KeyPressed(KeyCode.D))
                moveDirection.X = 1;

            moveDirection = Vector3.Normalize(moveDirection);
            moveDirection = Vector3.Transform(moveDirection, _camera.Transform.Rotation);
            moveDirection.Y = 0.0f;

            if (moveDirection.LengthSquared() > 0)
            {
                var currentRotation = _renderer.GameObject.Transform.Rotation;
                var targetRotation = Extentions.LookRotation(moveDirection, Vector3.UnitY);
                _renderer.GameObject.Transform.Rotation = Quaternion.Lerp(currentRotation, targetRotation, 10.0f * EduTime.DeltaTime);
                _body.Velocity = new Vector3(moveDirection.X * _moveSpeed, _body.Velocity.Y, moveDirection.Z * _moveSpeed);
            }

            ImGui.SetNextWindowPos(new Vector2(Screen.Size.X - 250, 50), ImGuiCond.Always);
            ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);

            ImGui.Begin($"##InvisibleWindow{GUID}", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);
            ImGui.TextColored(new Vector4(0, 1, 0, 1), $"Player: {PowerLevel}");
            ImGui.End();
        }
    }
}

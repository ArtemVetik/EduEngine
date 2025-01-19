using EduEngine;
using BananaParty.BehaviorTree;
using System.Numerics;

namespace BehaviorTreeDemo
{
    internal class FreeFromCharacter : Node
    {
        private const float MoveSpeed = 2.0f;

        private readonly GameObject _self;
        private readonly SharedVariable<Character> _character;
        private readonly Renderer _renderer;
        private readonly RigidBody _body;

        public FreeFromCharacter(GameObject self, SharedVariable<Character> sharedCharacter)
        {
            _self = self;
            _character = sharedCharacter;
            _renderer = _self.GetComponentsInChildren<Renderer>().First();
            _body = _self.GetComponent<RigidBody>();
        }

        public override NodeStatus OnExecute(float deltaTime)
        {
            if (_character.Value == null)
                throw new InvalidOperationException($"{nameof(_character.Value)} is null");

            Vector3 positionDifference = _character.Value.GameObject.Transform.Position - _self.Transform.Position;
            positionDifference.Y = 0f;
            Vector3 fleeDirection = Vector3.Normalize(-positionDifference);
            _body.Velocity = new Vector3(fleeDirection.X * MoveSpeed, _body.Velocity.Y, fleeDirection.Z * MoveSpeed);

            var currentRotation = _renderer.GameObject.Transform.Rotation;
            var targetRotation = Extentions.LookRotation(fleeDirection, Vector3.UnitY);
            _renderer.GameObject.Transform.Rotation = Quaternion.Lerp(currentRotation, targetRotation, 10.0f * EduTime.DeltaTime);

            return NodeStatus.Running;
        }
    }
}

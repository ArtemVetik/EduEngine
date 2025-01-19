using BananaParty.BehaviorTree;
using EduEngine;
using System.Numerics;

namespace BehaviorTreeDemo
{
    internal class ChaseObject<TObject> : Node where TObject : Component
    {
        private const float MoveSpeed = 5.0f;

        private readonly GameObject _self;
        private readonly SharedVariable<TObject> _target;
        private readonly RigidBody _body;
        private readonly Renderer _renderer;
        private readonly string _name;

        public ChaseObject(GameObject self, SharedVariable<TObject> target, string name = null)
        {
            _self = self;
            _target = target;
            _body = _self.GetComponent<RigidBody>();
            _renderer = _self.GetComponentsInChildren<Renderer>().First();
            _name = name;
        }

        public override string Name => _name ?? base.Name;

        public override NodeStatus OnExecute(float deltaTime)
        {
            if (_target.Value == null)
                throw new InvalidOperationException($"{nameof(_target.Value)} is null");

            Vector3 positionDifference = _target.Value.GameObject.Transform.Position - _self.Transform.Position;
            positionDifference.Y = 0f;
            Vector3 chaseDirection = Vector3.Normalize(positionDifference);
            _body.Velocity = new Vector3(chaseDirection.X * 5, _body.Velocity.Y, chaseDirection.Z * MoveSpeed);

            var currentRotation = _renderer.GameObject.Transform.Rotation;
            var targetRotation = Extentions.LookRotation(chaseDirection, Vector3.UnitY);
            _renderer.GameObject.Transform.Rotation = Quaternion.Lerp(currentRotation, targetRotation, 10.0f * EduTime.DeltaTime);

            return NodeStatus.Running;
        }
    }
}

using BananaParty.BehaviorTree;

namespace BehaviorTreeDemo
{
    internal class IsGreaterPowerLevel : Node
    {
        private readonly Character _self;
        private readonly SharedVariable<Character> _nearbyCharacter;

        public IsGreaterPowerLevel(Character self, SharedVariable<Character> sharedCharacter)
        {
            _self = self;
            _nearbyCharacter = sharedCharacter;
        }

        public override NodeStatus OnExecute(float deltaTime)
        {
            if (_nearbyCharacter.Value == null)
                throw new InvalidOperationException($"{nameof(_nearbyCharacter.Value)} is null");

            return _self.CanChase(_nearbyCharacter.Value) ? NodeStatus.Success : NodeStatus.Failure;
        }
    }
}

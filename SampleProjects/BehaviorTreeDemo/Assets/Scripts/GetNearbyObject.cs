using EduEngine;
using BananaParty.BehaviorTree;
using System.Numerics;

namespace BehaviorTreeDemo
{
    internal class GetNearbyObject<TNearbyObject> : Node where TNearbyObject : Component
    {
        private readonly GameObject _self;
        private readonly TNearbyObject[] _nearbyObjects;
        private readonly SharedVariable<TNearbyObject> _nearbyObject;
        private readonly float _detectDistance;
        private readonly string _name;

        public GetNearbyObject(GameObject self, TNearbyObject[] nearbyObjects, SharedVariable<TNearbyObject> sharedObject, float detectDistance, string name = null)
        {
            _self = self;
            _nearbyObjects = nearbyObjects;
            _detectDistance = detectDistance;
            _nearbyObject = sharedObject;
            _name = name;
        }

        public override string Name => _name ?? base.Name;

        public override NodeStatus OnExecute(float deltaTime)
        {
            _nearbyObject.Value = null;
            var minDistance = float.MaxValue;

            foreach (var nearbyObject in _nearbyObjects)
            {
                if (nearbyObject.GameObject == null || nearbyObject.GameObject.IsAlive == false)
                    continue;

                var distance = Vector3.Distance(nearbyObject.GameObject.Transform.Position, _self.Transform.Position);
                if (distance <= _detectDistance && distance < minDistance)
                {
                    minDistance = distance;
                    _nearbyObject.Value = nearbyObject;
                }
            }

            return _nearbyObject.Value != null ? NodeStatus.Success : NodeStatus.Failure;
        }
    }
}

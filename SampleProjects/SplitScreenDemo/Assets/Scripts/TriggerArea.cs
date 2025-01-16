using EduEngine;

namespace SplitScreenDemo
{
    public class TriggerArea : Component, IColliderCallbacks
    {
        [SerializeField] private GameObject _targetObject0;
        [SerializeField] private GameObject _targetObject1;
        [SerializeField] private GameObject _targetObject2;

        private List<ITriggerTarget> _targets;
        private Collider _entity = null;

        public TriggerArea(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _targets = new List<ITriggerTarget>();

            if (_targetObject0 != null)
                _targets.Add((ITriggerTarget)_targetObject0.GetComponents<Component>().First(c => c is ITriggerTarget));
            if (_targetObject1 != null)
                _targets.Add((ITriggerTarget)_targetObject1.GetComponents<Component>().First(c => c is ITriggerTarget));
            if (_targetObject2 != null)
                _targets.Add((ITriggerTarget)_targetObject2.GetComponents<Component>().First(c => c is ITriggerTarget));
        }

        public void OnCollisionEnter(CollisionData collisionData) { }

        public void OnCollisionExit(CollisionData collisionData) { }

        public void OnTriggerEnter(Collider other)
        {
            if (_entity != null)
                return;

            _entity = other;
            _targets.ForEach(t => t.StartAction());
        }

        public void OnTriggerExit(Collider other)
        {
            if (_entity != other)
                return;

            _targets.ForEach(t => t.EndAction());
            _entity = null;
        }
    }
}

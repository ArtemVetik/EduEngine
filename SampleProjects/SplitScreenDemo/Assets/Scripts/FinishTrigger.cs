using EduEngine;

namespace SplitScreenDemo
{
    public class FinishTrigger : Component, IColliderCallbacks
    {
        private List<PlayerMovement> _players = new List<PlayerMovement>();
        private List<ParticleSystem> _fireworks = new List<ParticleSystem>();

        public FinishTrigger(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _fireworks = GameObject.GetComponentsInChildren<ParticleSystem>().ToList();
        }

        public void OnTriggerEnter(Collider other)
        {
            if (_players.Count == 2)
                return;

            var player = other.GameObject.GetComponentsInParent<PlayerMovement>().First();

            if (_players.Contains(player))
                return;

            _players.Add(player);

            if (_players.Count == 2)
            {
                foreach (var firework in _fireworks)
                    firework.SetEmissionState(true);
            }
        }

        public void OnTriggerExit(Collider other) { }
        public void OnCollisionEnter(CollisionData collisionData) { }
        public void OnCollisionExit(CollisionData collisionData) { }
    }
}

using EduEngine;

namespace BehaviorTreeDemo
{
    public class Character : Component, IColliderCallbacks, ICombatant
    {
        private int _powerLevel;

        public Character(GameObject parent)
            : base(parent)
        {
            _powerLevel = 3;
        }

        public int PowerLevel => _powerLevel;

        public bool CanChase(ICombatant other)
        {
            return PowerLevel >= other.PowerLevel + 2;
        }

        public void Die()
        {
            GameObject.Destroy();
        }

        public void OnCollisionEnter(CollisionData collisionData)
        {
            var otherCollider = (Collider)collisionData.Other;

            var otherCharacter = otherCollider.GameObject.GetComponentsInParent<Component>().FirstOrDefault(c => c is ICombatant);

            if (otherCharacter != null)
            {
                var combatant = (ICombatant)otherCharacter;

                if (CanChase(combatant))
                {
                    combatant.Die();
                    _powerLevel++;
                }
            }
        }

        public void OnCollisionExit(CollisionData collisionData) { }
        public void OnTriggerEnter(Collider other) { }
        public void OnTriggerExit(Collider other) { }
    }
}

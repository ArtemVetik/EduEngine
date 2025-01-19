namespace BehaviorTreeDemo
{
    public interface ICombatant
    {
        int PowerLevel { get; }
        void Die();
    }
}

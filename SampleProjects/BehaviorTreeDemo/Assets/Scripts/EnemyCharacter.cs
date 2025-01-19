using BananaParty.BehaviorTree;
using EduEngine;
using ImGuiNET;
using System.Numerics;

namespace BehaviorTreeDemo
{
    public class EnemyCharacter : Character
    {
        private INode _behaviorTree;
        private Character[] _characters;
        private Mob[] _mobs;
        private int _selfIndex;
        private bool _showTree;
        private bool _active;

        public EnemyCharacter(GameObject parent)
            : base(parent)
        { }

        public override void OnAddComponent()
        {

        }

        public override void OnGameStart()
        {
            var nearbyCharacter = new SharedVariable<Character>();
            var nearbyMob = new SharedVariable<Mob>();

            _behaviorTree = new Repeat(new ReactiveSelector(new INode[]
            {
                new ReactiveSequence(new INode[]
                {
                   new GetNearbyObject<Character>(GameObject, _characters, nearbyCharacter, 100, "GetNearbyCharacter"),
                   new IsGreaterPowerLevel(this, nearbyCharacter),
                   new ChaseObject<Character>(GameObject, nearbyCharacter, "ChaseCharacter")
                }),
                new ReactiveSequence(new INode[]
                {
                    new GetNearbyObject<Mob>(GameObject, _mobs, nearbyMob, 100, "GetNearbyMob"),
                    new ChaseObject<Mob>(GameObject, nearbyMob, "ChaseMob"),
                }),
                new Sequence(new INode[]
                {
                    new Wait(5)
                })
            }));
        }

        public override void Update()
        {
            if (_active)
                _behaviorTree.Execute(EduTime.DeltaTime);

            if (_showTree)
                RenderTree();

            RenderPower();
        }

        public void Initialize(int index, Character[] characters, Mob[] mobs)
        {
            _selfIndex = index;
            _characters = characters;
            _mobs = mobs;
        }

        private void RenderTree()
        {
            var textTree = new TextTreeGraph(GameObject.Name);
            _behaviorTree.WriteToGraph(textTree);

            ImGui.SetNextWindowPos(new Vector2(100, 100), ImGuiCond.Always);
            ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);

            ImGui.Begin($"##InvisibleWindow{GUID}", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);

            var treeString = textTree.ToString();

            var lines = treeString.Split("\n");

            foreach (string line in lines)
            {
                if (line.EndsWith("R"))
                {
                    ImGui.TextColored(new Vector4(0.0f, 1.0f, 0.0f, 1.0f), line);
                }
                else
                {
                    ImGui.Text(line);
                }
            }

            ImGui.End();
        }

        private void RenderPower()
        {
            ImGui.SetNextWindowPos(new Vector2(Screen.Size.X - 250, 70 + _selfIndex * 20), ImGuiCond.Always);
            ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);

            ImGui.Begin($"##InvisiblePowerWindow{GUID}", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);

            ImGui.TextColored(new Vector4(1, 1, 0, 1), $"Enemy: {PowerLevel}");
            ImGui.SameLine();
            ImGui.Checkbox("Show", ref _showTree);
            ImGui.SameLine();
            ImGui.Checkbox("Active", ref _active);

            ImGui.End();
        }
    }
}

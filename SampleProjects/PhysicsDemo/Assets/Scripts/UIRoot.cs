using EduEngine;
using ImGuiNET;
using System.Numerics;

namespace PhysicsDemo
{
    internal class UIRoot : Component
    {
        [SerializeField] private GameObject _sphereGunRoot;

        private SphereGun _sphereGun;
        private float _gunPower = 60.0f;

        public UIRoot(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            OverlayUI.Initialize();
        }

        public override void OnGameStart()
        {
            _sphereGun = _sphereGunRoot.GetComponent<SphereGun>();
            _sphereGun.SetPower(_gunPower);
        }

        public override void Update()
        {
            DrawCrosshair();

            ImGui.SetNextWindowPos(new Vector2(0.05f * Screen.Size.X, 0.1f * Screen.Size.Y), ImGuiCond.Always);
            ImGui.SetNextWindowSize(Vector2.Zero, ImGuiCond.Always);

            ImGui.Begin("##InvisibleWindow", ImGuiWindowFlags.NoTitleBar | ImGuiWindowFlags.NoResize | ImGuiWindowFlags.NoMove | ImGuiWindowFlags.NoBackground);

            ImGui.Text("----PHYSICS DEMO----\n" +
                       "Move:   'W' 'A' 'S' 'D' + 'Q' 'E'\n" +
                       "Rotate: RMB + mouse move\n" +
                       "Fire:   Space");

            ImGui.Dummy(new Vector2(0, 50));

            bool reload = false;
            if (ImGui.Button("Reload Scene"))
            {
                reload = true;
            }

            ImGui.Dummy(new Vector2(0, 20));

            bool sphereProjectile = _sphereGun.SphereProjectile;
            if (ImGui.Checkbox("Sphere Projectile", ref sphereProjectile))
                _sphereGun.SetSphereProjectile(sphereProjectile);

            if (ImGui.SliderFloat($"Gun Power##{GUID}", ref _gunPower, 1.0f, 120.0f, "Value: %.2f"))
                _sphereGun.SetPower(_gunPower);

            if (ImGui.Button($"Clear##{GUID}"))
                _sphereGun.ClearInstances();
            ImGui.SameLine();
            ImGui.Text($"Spawned: {_sphereGun.SpawnedInstancesCount}");

            ImGui.End();

            if (reload)
                SceneExtentions.ReloadCurrentScene();
        }

        public override void OnDestroy()
        {
            OverlayUI.Destroy();
        }

        private void DrawCrosshair()
        {
            var io = ImGui.GetIO();
            Vector2 screenSize = io.DisplaySize;

            Vector2 center = new Vector2(screenSize.X / 2, screenSize.Y / 2);

            var drawList = ImGui.GetBackgroundDrawList();

            float radius = 2.0f;

            uint color = ImGui.GetColorU32(new Vector4(0.0f, 1.0f, 0.0f, 1.0f));

            drawList.AddCircleFilled(center, radius, color);
        }
    }
}

using EduEngine;

namespace BehaviorTreeDemo
{
    public class UIRoot : Character
    {
        public UIRoot(GameObject parent)
            : base(parent)
        { }

        public override void OnCreate()
        {
            OverlayUI.Initialize();
        }

        public override void OnDestroy()
        {
            OverlayUI.Destroy();
        }
    }
}

using EduEngine;

namespace SplitScreenDemo
{
    public class UIRoot : Component
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

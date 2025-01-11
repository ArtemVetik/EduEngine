using EduEngine;

namespace PhysicsDemo
{
    internal class CatapultBox : Component, IColliderCallbacks
    {
        private RigidBody _body;
        private bool _activated;

        public CatapultBox(GameObject parent)
            : base(parent)
        { }

        public override void OnGameStart()
        {
            _body = GameObject.GetComponent<RigidBody>();
        }

        public void OnCollisionEnter(CollisionData collisionData)
        {
            if (_activated)
                return;

            _body.IsStatic = false;
            _activated = true;
        }

        public void OnCollisionExit(CollisionData collisionData)
        {

        }

        public void OnTriggerEnter(Collider other)
        {

        }

        public void OnTriggerExit(Collider other)
        {

        }
    }
}

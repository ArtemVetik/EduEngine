#pragma once
#include "../../Physics/IPhysicsShape.h"
#include "ColliderData.h"

namespace EduEngine
{
	class NativePhysicsShapeUnmanaged
	{
	public:
		static void DebugDraw(DirectX::SimpleMath::Matrix transformMatrix, ColliderShape geometry);
	};

	public ref class NativePhysicsShapeWrapper
	{
	private:
		IPhysicsShape* m_NativeShape;

	public:
		NativePhysicsShapeWrapper(IPhysicsShape* nativeShape);
		~NativePhysicsShapeWrapper();
		!NativePhysicsShapeWrapper();

		void SetGeometry(ColliderData^ shape);
		void SetMaterial(float staticFriction, float dynamicFriction, float restitution);

		void DebugDraw(System::Numerics::Matrix4x4 worldMatrix);

		IPhysicsShape* GetNative() { return m_NativeShape; }
	};
}
#pragma once
#include "../../Physics/IPhysicsShape.h"
#include "ColliderData.h"

namespace EduEngine
{
	using namespace System;
	using namespace System::Numerics;
	using namespace System::Runtime::InteropServices;

	class NativePhysicsShapeUnmanaged
	{
	public:
		static void DebugDraw(DirectX::SimpleMath::Matrix transformMatrix, ColliderShape geometry);
	};

	public value class CollisionContactData
	{
	public:
		Vector3 Point;
		Vector3 Normal;
		Vector3 Impulse;
		float Separation;
	};

	public ref class CollisionData
	{
	public:
		Object^ Other;
		int ContactCount;
		array<CollisionContactData>^ Contacts;
	};

	private ref class NativePhysicsShapeWrapper
	{
	public:
		NativePhysicsShapeWrapper(ColliderData^ shape, Object^ managedObj);
		~NativePhysicsShapeWrapper();
		!NativePhysicsShapeWrapper();

		void SetLocalTransform(Vector3 position, Quaternion rotation);
		void SetGeometry(ColliderData^ shape);
		void SetMaterial(float staticFriction, float dynamicFriction, float restitution);
		void SetTrigger(bool isTrigger);

		void SetTriggerEnterCallback(Action<Object^>^ callback);
		void SetTriggerExitCallback(Action<Object^>^ callback);
		void SetCollisionEnterCallback(Action<CollisionData^>^ callback);
		void SetCollisionExitCallback(Action<CollisionData^>^ callback);

		void DebugDraw(System::Numerics::Matrix4x4 worldMatrix);

		IPhysicsShape* GetNative() { return m_NativeShape; }

	private:
		delegate void ShapeTriggerCallbackDelegate(void* otherObject);
		delegate void ShapeCollisionCallbackDelegate(PhysXCollisionData collisionData);

		void OnTriggerEnter(void* otherObject);
		void OnTriggerExit(void* otherObject);
		void OnCollisionEnter(PhysXCollisionData otherObject);
		void OnCollisionExit(PhysXCollisionData otherObject);

		CollisionData^ CreateCollisionData(PhysXCollisionData nativeData);

	private:
		IPhysicsShape* m_NativeShape;
		GCHandle m_ManagedHandle;
		Action<Object^>^ m_TriggerEnterCallback;
		Action<Object^>^ m_TriggerExitCallback;
		Action<CollisionData^>^ m_CollisionEnterCallback;
		Action<CollisionData^>^ m_CollisionExitCallback;
		ShapeTriggerCallbackDelegate^ m_TriggerEnterInternal;
		ShapeTriggerCallbackDelegate^ m_TriggerExitInternal;
		ShapeCollisionCallbackDelegate^ m_CollisionEnterInternal;
		ShapeCollisionCallbackDelegate^ m_CollisionExitInternal;
	};
}
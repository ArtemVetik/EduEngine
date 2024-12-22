#pragma once
#include "../../Physics/IPhysicsShape.h"
#include "ColliderData.h"

namespace EduEngine
{
	using namespace System;
	using namespace System::Runtime::InteropServices;

	class NativePhysicsShapeUnmanaged
	{
	public:
		static void DebugDraw(DirectX::SimpleMath::Matrix transformMatrix, ColliderShape geometry);
	};

	private ref class NativePhysicsShapeWrapper
	{
	public:
		NativePhysicsShapeWrapper(ColliderData^ shape, Object^ managedObj);
		~NativePhysicsShapeWrapper();
		!NativePhysicsShapeWrapper();

		void SetGeometry(ColliderData^ shape);
		void SetMaterial(float staticFriction, float dynamicFriction, float restitution);
		void SetTrigger(bool isTrigger);

		void SetTriggerEnterCallback(Action<Object^>^ callback);
		void SetTriggerExitCallback(Action<Object^>^ callback);

		void DebugDraw(System::Numerics::Matrix4x4 worldMatrix);

		IPhysicsShape* GetNative() { return m_NativeShape; }

	private:
		delegate void ShapeCallbackDelegate(void* otherObject);

		void OnTriggerEnter(void* otherObject);
		void OnTriggerExit(void* otherObject);

	private:
		IPhysicsShape* m_NativeShape;
		GCHandle m_ManagedHandle;
		Action<Object^>^ m_TriggerEnterCallback;
		Action<Object^>^ m_TriggerExitCallback;
		ShapeCallbackDelegate^ m_TriggerEnterInternal;
		ShapeCallbackDelegate^ m_TriggerExitInternal;
	};
}
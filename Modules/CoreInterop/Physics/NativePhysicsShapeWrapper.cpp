#include "NativePhysicsShapeWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	NativePhysicsShapeWrapper::NativePhysicsShapeWrapper(ColliderData^ shape, Object^ managedObj)
	{
		m_ManagedHandle = GCHandle::Alloc(managedObj);
		m_NativeShape = CoreSystems::GetInstance()->GetPhysicsWorld()->CreateShape(shape->ToNative(), GCHandle::ToIntPtr(m_ManagedHandle).ToPointer());
	}

	NativePhysicsShapeWrapper::~NativePhysicsShapeWrapper()
	{
		this->!NativePhysicsShapeWrapper();
	}

	NativePhysicsShapeWrapper::!NativePhysicsShapeWrapper()
	{
		if (!m_NativeShape)
			return; 

		CoreSystems::GetInstance()->GetPhysicsWorld()->RemoveShape(m_NativeShape);
		m_ManagedHandle.Free();

		m_NativeShape = nullptr;
	}

	void NativePhysicsShapeWrapper::SetGeometry(ColliderData^ shape)
	{
		m_NativeShape->SetGeometry(shape->ToNative());
	}

	void NativePhysicsShapeWrapper::SetMaterial(float staticFriction, float dynamicFriction, float restitution)
	{
		m_NativeShape->SetMaterial(staticFriction, dynamicFriction, restitution);
	}

	void NativePhysicsShapeWrapper::SetTrigger(bool isTrigger)
	{
		m_NativeShape->SetTrigger(isTrigger);
	}

	void NativePhysicsShapeWrapper::SetTriggerEnterCallback(Action<Object^>^ callback)
	{
		m_TriggerEnterCallback = callback;

		m_TriggerEnterInternal = gcnew ShapeCallbackDelegate(this, &NativePhysicsShapeWrapper::OnTriggerEnter);
		IntPtr rahPtr = Marshal::GetFunctionPointerForDelegate(m_TriggerEnterInternal);
		auto callbackFunctionCpp = static_cast<void(__stdcall*)(void*)>(rahPtr.ToPointer());

		m_NativeShape->SetTriggerEnterCallback(callbackFunctionCpp);
	}

	void NativePhysicsShapeWrapper::SetTriggerExitCallback(Action<Object^>^ callback)
	{
		m_TriggerExitCallback = callback;

		m_TriggerExitInternal = gcnew ShapeCallbackDelegate(this, &NativePhysicsShapeWrapper::OnTriggerExit);
		IntPtr rahPtr = Marshal::GetFunctionPointerForDelegate(m_TriggerExitInternal);
		auto callbackFunctionCpp = static_cast<void(__stdcall*)(void*)>(rahPtr.ToPointer());

		m_NativeShape->SetTriggerExitCallback(callbackFunctionCpp);
	}

	void NativePhysicsShapeWrapper::OnTriggerEnter(void* otherObject)
	{
		auto objectGCHandle = GCHandle::FromIntPtr(IntPtr(otherObject));
		m_TriggerEnterCallback->Invoke(objectGCHandle.Target);
	}

	void NativePhysicsShapeWrapper::OnTriggerExit(void* otherObject)
	{
		auto objectGCHandle = GCHandle::FromIntPtr(IntPtr(otherObject));
		m_TriggerExitCallback->Invoke(objectGCHandle.Target);
	}

	void NativePhysicsShapeWrapper::DebugDraw(System::Numerics::Matrix4x4 worldMatrix)
	{
		auto transformMatrix = DirectX::SimpleMath::Matrix(
			worldMatrix.M11, worldMatrix.M12, worldMatrix.M13, worldMatrix.M14,
			worldMatrix.M21, worldMatrix.M22, worldMatrix.M23, worldMatrix.M24,
			worldMatrix.M31, worldMatrix.M32, worldMatrix.M33, worldMatrix.M34,
			worldMatrix.M41, worldMatrix.M42, worldMatrix.M43, worldMatrix.M44
		);

		NativePhysicsShapeUnmanaged::DebugDraw(transformMatrix, m_NativeShape->GetGeometry());
	}

	void NativePhysicsShapeUnmanaged::DebugDraw(DirectX::SimpleMath::Matrix transformMatrix, ColliderShape geometry)
	{
		IDebugRendererSystem* debugRenderer = CoreSystems::GetInstance()->GetRenderEngine()->GetDebugRender();

		if (geometry.type == ColliderType::Box)
		{
			auto boundingBox = DirectX::BoundingBox({}, { geometry.box.width, geometry.box.height, geometry.box.depth });
			debugRenderer->DrawBoundingBox(boundingBox, transformMatrix);
		}
		else if (geometry.type == ColliderType::Capsule)
		{
			debugRenderer->DrawCapsule(geometry.capsule.radius, geometry.capsule.halfHeight, DirectX::Colors::Green, transformMatrix, 16);
		}
		else if (geometry.type == ColliderType::Sphere)
		{
			debugRenderer->DrawSphere(geometry.sphere.radius, DirectX::Colors::Green, transformMatrix, 16);
		}
	}
}
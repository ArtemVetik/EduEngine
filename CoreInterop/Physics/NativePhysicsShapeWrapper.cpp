#include "NativePhysicsShapeWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	NativePhysicsShapeWrapper::NativePhysicsShapeWrapper(ColliderData^ shape)
	{
		m_NativeShape = CoreSystems::GetInstance()->GetPhysicsWorld()->CreateShape(shape->ToNative());
	}

	NativePhysicsShapeWrapper::~NativePhysicsShapeWrapper()
	{
		this->!NativePhysicsShapeWrapper();
	}

	NativePhysicsShapeWrapper::!NativePhysicsShapeWrapper()
	{
		CoreSystems::GetInstance()->GetPhysicsWorld()->RemoveShape(m_NativeShape);
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
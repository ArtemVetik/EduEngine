#include "PhysicsWorldInterop.h"

namespace EduEngine
{
	NativePhysicsObjectWrapper^ PhysicsWorldInterop::AddBody(Numerics::Vector3 position, Numerics::Quaternion rotation, bool isStatic)
	{
		auto body = CoreSystems::GetInstance()->GetPhysicsWorld()->AddBody(
			{ position.X, position.Y, position.Z },
			{ rotation.X, rotation.Y, rotation.Z, rotation.W },
			isStatic
		);

		return gcnew NativePhysicsObjectWrapper(body);
	}

	void PhysicsWorldInterop::RemoveBody(NativePhysicsObjectWrapper^ body)
	{
		CoreSystems::GetInstance()->GetPhysicsWorld()->RemoveBody(body->GetNativeObject());
	}

	NativePhysicsShapeWrapper^ PhysicsWorldInterop::CreateShape(ColliderData^ shape)
	{
		auto nativeShape = CoreSystems::GetInstance()->GetPhysicsWorld()->CreateShape(shape->ToNative());
		return gcnew NativePhysicsShapeWrapper(nativeShape);
	}

	void PhysicsWorldInterop::RemoveShape(NativePhysicsShapeWrapper^ shape)
	{
		CoreSystems::GetInstance()->GetPhysicsWorld()->RemoveShape(shape->GetNative());
	}
}
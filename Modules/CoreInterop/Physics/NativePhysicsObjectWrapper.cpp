#include "NativePhysicsObjectWrapper.h"
#include "../../RenderEngine/IDebugRendererSystem.h"
#include "../../EduMath/SimpleMath.h"

namespace EduEngine
{
	NativePhysicsObjectWrapper::NativePhysicsObjectWrapper(Numerics::Vector3 position, Numerics::Quaternion rotation, bool isStatic)
	{
		m_NativeObject = CoreSystems::GetInstance()->GetPhysicsWorld()->AddBody(
			{ position.X, position.Y, position.Z },
			{ rotation.X, rotation.Y, rotation.Z, rotation.W },
			isStatic
		);
	}

	NativePhysicsObjectWrapper::~NativePhysicsObjectWrapper()
	{
		this->!NativePhysicsObjectWrapper();
	}

	NativePhysicsObjectWrapper::!NativePhysicsObjectWrapper()
	{
		CoreSystems::GetInstance()->GetPhysicsWorld()->RemoveBody(m_NativeObject);
	}

	void NativePhysicsObjectWrapper::SetGlobalTransform(Numerics::Vector3 position, Numerics::Quaternion rotation)
	{
		m_NativeObject->SetGlobalTransform(
			{ position.X, position.Y, position.Z },
			{ rotation.X, rotation.Y, rotation.Z, rotation.W }
		);
	}

	void NativePhysicsObjectWrapper::FreezePosition(bool x, bool y, bool z)
	{
		m_NativeObject->FreezePosition(x, y, z);
	}

	void NativePhysicsObjectWrapper::FreezeRotation(bool x, bool y, bool z)
	{
		m_NativeObject->FreezeRotation(x, y, z);
	}

	void NativePhysicsObjectWrapper::SetMass(float mass)
	{
		m_NativeObject->SetMass(mass);
	}

	void NativePhysicsObjectWrapper::SetLinearVelocity(Numerics::Vector3 velocity)
	{
		m_NativeObject->SetLinearVelocity({ velocity.X, velocity.Y, velocity.Z });
	}

	void NativePhysicsObjectWrapper::AddForce(Numerics::Vector3 force, ForceMode forceMode)
	{
		m_NativeObject->AddForce({ force.X, force.Y, force.Z }, (NativeForceMode)forceMode);
	}

	void NativePhysicsObjectWrapper::AttachShape(NativePhysicsShapeWrapper^ shape)
	{
		m_NativeObject->AttachShape(shape->GetNative());
	}

	void NativePhysicsObjectWrapper::DetachShape(NativePhysicsShapeWrapper^ shape)
	{
		m_NativeObject->DetachShape(shape->GetNative());
	}

	Numerics::Vector3 NativePhysicsObjectWrapper::GetPosition()
	{
		auto pos = m_NativeObject->GetPosition();
		return Numerics::Vector3(pos.x, pos.y, pos.z);
	}

	Numerics::Quaternion NativePhysicsObjectWrapper::GetRotation()
	{
		auto quat = m_NativeObject->GetRotation();
		return Numerics::Quaternion(quat.x, quat.y, quat.z, quat.w);
	}

	Numerics::Vector3 NativePhysicsObjectWrapper::GetLinearVelocity()
	{
		auto velocity = m_NativeObject->GetLinearVelocity();
		return System::Numerics::Vector3(velocity.x, velocity.y, velocity.z);
	}
}
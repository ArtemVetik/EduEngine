#include "NativePhysicsObjectWrapper.h"
#include "../../RenderEngine/IDebugRendererSystem.h"
#include "../../EduMath/SimpleMath.h"

namespace EduEngine
{
	NativePhysicsObjectWrapper::NativePhysicsObjectWrapper(IPhysicsObject* nativePtr)
	{
		m_NativeObject = nativePtr;
	}

	NativePhysicsObjectWrapper::~NativePhysicsObjectWrapper()
	{
		this->!NativePhysicsObjectWrapper();
	}

	NativePhysicsObjectWrapper::!NativePhysicsObjectWrapper()
	{
		CoreSystems::GetInstance()->GetPhysicsWorld()->RemoveBody(m_NativeObject);
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
}
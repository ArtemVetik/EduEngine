#pragma once
#include "../../Physics/IPhysicsObject.h"
#include "NativePhysicsShapeWrapper.h"
#include "ColliderData.h"

using namespace System;

namespace EduEngine
{
	public enum class ForceMode
	{
		FORCE,				//!< parameter has unit of mass * length / time^2, i.e., a force
		IMPULSE,			//!< parameter has unit of mass * length / time, i.e., force * time
		VELOCITY_CHANGE,	//!< parameter has unit of length / time, i.e., the effect is mass independent: a velocity change.
		ACCELERATION
	};

	private ref class NativePhysicsObjectWrapper
	{
	private:
		IPhysicsObject* m_NativeObject;

	public:
		NativePhysicsObjectWrapper(Numerics::Vector3 position, Numerics::Quaternion rotation, bool isStatic);
		~NativePhysicsObjectWrapper();
		!NativePhysicsObjectWrapper();

		void SetGlobalTransform(Numerics::Vector3 position, Numerics::Quaternion rotation);
		void FreezePosition(bool x, bool y, bool z);
		void FreezeRotation(bool x, bool y, bool z);
		void SetMass(float mass);
		void SetLinearVelocity(Numerics::Vector3 velocity);
		void SetAngularVelocity(Numerics::Vector3 velocity);
		void AddForce(Numerics::Vector3 force, ForceMode forceMode);
		void AttachShape(NativePhysicsShapeWrapper^ shape);
		void DetachShape(NativePhysicsShapeWrapper^ shape);
		Numerics::Vector3 GetPosition();
		Numerics::Quaternion GetRotation();
		Numerics::Vector3 GetLinearVelocity();
		Numerics::Vector3 GetAngularVelocity();

		IPhysicsObject* GetNativeObject() { return m_NativeObject; }
	};
}
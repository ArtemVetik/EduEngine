#pragma once
#include "../CoreSystems.h"
#include "NativePhysicsObjectWrapper.h"
#include "NativePhysicsShapeWrapper.h"
#include "ColliderData.h"

using namespace System;

namespace EduEngine
{
	private ref class PhysicsWorldInterop
	{
	public:
		static NativePhysicsObjectWrapper^ AddBody(Numerics::Vector3 position, Numerics::Quaternion rotation, bool isStatic);
		static void RemoveBody(NativePhysicsObjectWrapper^ body);
		static NativePhysicsShapeWrapper^ CreateShape(ColliderData^ shape);
		static void RemoveShape(NativePhysicsShapeWrapper^ shape);
	};
}
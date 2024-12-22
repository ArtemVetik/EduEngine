#pragma once
#include "framework.h"

namespace EduEngine
{
	enum class PHYSICS_API NativeForceMode
	{
		FORCE,				//!< parameter has unit of mass * length / time^2, i.e., a force
		IMPULSE,			//!< parameter has unit of mass * length / time, i.e., force * time
		VELOCITY_CHANGE,	//!< parameter has unit of length / time, i.e., the effect is mass independent: a velocity change.
		ACCELERATION
	};

	enum class PHYSICS_API ColliderType
	{
		Box,
		Sphere,
		Capsule
	};

	struct PHYSICS_API ColliderShape
	{
		ColliderType type;
		union {
			struct {
				float width, height, depth;
			} box;
			struct {
				float radius;
			} sphere;
			struct {
				float radius, halfHeight;
			} capsule;
		};

		ColliderShape(float w, float h, float d) : type(ColliderType::Box), box{ w, h, d } {}
		ColliderShape(float r) : type(ColliderType::Sphere), sphere{ r } {}
		ColliderShape(float r, float h) : type(ColliderType::Capsule), capsule{ r, h } {}
	};
}
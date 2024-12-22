#pragma once
#include "../CoreSystems.h"

using namespace System;

namespace EduEngine
{
	private ref class ColliderData abstract
	{
	public:
		virtual ColliderShape ToNative() abstract;
	};

	private ref class BoxColliderData : public ColliderData
	{
	private:
		float m_Width;
		float m_Height;
		float m_Depth;

	public:
		BoxColliderData(float w, float h, float d) :
			m_Width(w), m_Height(h), m_Depth(d)
		{
		}

		ColliderShape ToNative() override
		{
			return ColliderShape(m_Width, m_Height, m_Depth);
		}
	};

	private ref class SphereColliderData : public ColliderData
	{
	private:
		float m_Radius;

	public:
		SphereColliderData(float r) :
			m_Radius(r)
		{
		}

		ColliderShape ToNative() override
		{
			return ColliderShape(m_Radius);
		}
	};
}
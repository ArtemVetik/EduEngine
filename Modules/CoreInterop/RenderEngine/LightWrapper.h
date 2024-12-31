#pragma once
#include "../../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	using namespace System::Numerics;

	class NativeLightUnmanaged
	{
	public:
		static void DebugDraw(Light* light);
	};

	public enum class LightType
	{
		Directional = 0,
		Point = 1,
		Spotlight = 2,
	};

	private ref class LightWrapper
	{
	public:
		LightWrapper();
		~LightWrapper();
		!LightWrapper();

		void SetLightType(LightType value) { if (m_NativeLight) m_NativeLight->LightType = (Light::Type)value; }
		void SetPosition(Vector3 value) { if (m_NativeLight) m_NativeLight->Position = { value.X, value.Y, value.Z }; }
		void SetDirection(Vector3 value) { if (m_NativeLight) m_NativeLight->Direction = { value.X, value.Y, value.Z }; }
		void SetStrength(Vector3 value) { if (m_NativeLight) m_NativeLight->Strength = { value.X, value.Y, value.Z }; }
		void SetFalloffStart(float value) { if (m_NativeLight) m_NativeLight->FalloffStart = value; }
		void SetFalloffEnd(float value) { if (m_NativeLight) m_NativeLight->FalloffEnd = value; }
		void SetSpotPower(float value) { if (m_NativeLight) m_NativeLight->SpotPower = value; }

		void DebugDraw();

	private:
		Light* m_NativeLight;
	};
}
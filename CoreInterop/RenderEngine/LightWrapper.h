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

		void SetLightType(LightType value) { m_NativeLight->LightType = (Light::Type)value; }
		void SetPosition(Vector3 value) { m_NativeLight->Position = { value.X, value.Y, value.Z }; }
		void SetDirection(Vector3 value) { m_NativeLight->Direction = { value.X, value.Y, value.Z }; }
		void SetStrength(Vector3 value) { m_NativeLight->Strength = { value.X, value.Y, value.Z }; }
		void SetFalloffStart(float value) { m_NativeLight->FalloffStart = value; }
		void SetFalloffEnd(float value) { m_NativeLight->FalloffEnd = value; }
		void SetSpotPower(float value) { m_NativeLight->SpotPower = value; }

		void DebugDraw();

	private:
		Light* m_NativeLight;
	};
}
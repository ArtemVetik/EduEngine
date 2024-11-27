#pragma once
#include "../../RenderEngine/Camera.h"

namespace EduEngine
{
	using namespace System::Numerics;
	
	private ref class NativeCameraWrapper
	{
	private:
		Camera* m_NativeCamera;

	public:
		NativeCameraWrapper();
		~NativeCameraWrapper();
		!NativeCameraWrapper();

		void Update(Vector3 look, Vector3 right, Vector3 up, Vector3 pos);
		void Render();

		Camera* GetNative() { return m_NativeCamera; }
	};
}
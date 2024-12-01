#pragma once
#include "../../RenderEngine/Camera.h"

namespace EduEngine
{
	using namespace System::Numerics;
	
	class NativeCameraWrapperUnmanaged
	{
	public:
		static void DebugDraw(Camera* nativeCamera);
	};

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
		void DebugRender();

		void SetProjectionMatrix(float fov, float nearView, float farView);

		System::Numerics::Matrix4x4 GetView();
		System::Numerics::Matrix4x4 GetProjection();
		float GetFov();
		float GetNear();
		float GetFar();
		Camera* GetNative() { return m_NativeCamera; }
	};
}
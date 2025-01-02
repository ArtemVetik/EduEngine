#pragma once
#include "../../RenderEngine/Camera.h"
#include "TextureWrapper.h"

namespace EduEngine
{
	using namespace System::Numerics;
	
	class NativeCameraWrapperUnmanaged
	{
	public:
		static void DrawFrustrum(Camera* nativeCamera);
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
		void DrawFrustrum();

		void SetProjectionMatrix(float fov, float nearView, float farView);
		void SetViewport(Vector4 viewport);
		void SetBackgroundColor(Vector4 color);
		void SetDebugRenderEnable(bool value);
		void SetSkybox(TextureWrapper^ texture);

		Matrix4x4 GetView();
		Matrix4x4 GetProjection();
		Vector4 GetViewport();
		Vector4 GetBackgroundColor();
		bool IsDebugRenderEnabled();

		float GetFov();
		float GetNear();
		float GetFar();
		Camera* GetNative() { return m_NativeCamera; }
	};
}
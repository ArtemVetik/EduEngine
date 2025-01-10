#pragma once

namespace EduEngine
{
	using namespace DirectX;

	class Camera;
	class IRenderObject;

	class RENDERENGINE_API RayPicking
	{
	public:
		static bool Intersect(Camera* camera, IRenderObject* renderObject, XMFLOAT2 screenSize, XMFLOAT2 screenPos, float& dist);
	};
}
#pragma once
#include "framework.h"

namespace EduEngine
{
	class RENDERENGINE_API IDebugRendererSystem
	{
	public:
		virtual void DrawBoundingBox(const DirectX::BoundingBox& box, DirectX::XMVECTOR color = DirectX::Colors::Green) = 0;
		virtual void DrawBoundingBox(const DirectX::BoundingBox& box, DirectX::XMMATRIX transform, DirectX::XMVECTOR color = DirectX::Colors::Green) = 0;
		virtual void DrawCapsule(const double& radius, const double& halfHeight, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density) = 0;
		virtual void DrawSphere(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density) = 0;
		virtual void DrawCircle(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density) = 0;
		virtual void DrawArrow(const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3& p1, const DirectX::XMVECTOR& color, const DirectX::XMFLOAT3& n) = 0;
		virtual void DrawPoint(const DirectX::XMFLOAT3& pos, const float& size, const DirectX::XMVECTOR& color) = 0;
		virtual void DrawLine(const DirectX::XMFLOAT3& pos0, const DirectX::XMFLOAT3& pos1, const DirectX::XMVECTOR& color = DirectX::Colors::Green) = 0;
		virtual void DrawLine(const DirectX::XMVECTOR& pos0, const DirectX::XMVECTOR& pos1, const DirectX::XMVECTOR& color = DirectX::Colors::Green) = 0;
		virtual void DrawPlane(const DirectX::XMFLOAT4& p, const DirectX::XMVECTOR& color, float sizeWidth, float sizeNormal, bool drawCenterCross) = 0;
		virtual void DrawFrustrum(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj) = 0;
	};
}